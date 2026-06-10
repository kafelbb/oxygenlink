#pragma once
#include "icongrabber.h"
#include "logger.h"
#include "extadb.h"

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_set>

#include <windows.h>

#include "SFML/Graphics.hpp"

namespace fs = std::filesystem;

std::vector<std::string> get_all_apk_paths(const std::string& packageName) {
	std::vector<std::string> paths;
	std::string output = exec_com("tools\\scrcpy\\adb shell pm path " + packageName);

	output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
	std::istringstream stream(output);
	std::string line;

	while (std::getline(stream, line)) {
		if (line.find("package:") == 0) {
			paths.push_back(line.substr(8));
		}
	}
	return paths;
}

void check_cached() {
	if (!fs::is_directory("cached/")) {
		log_info("prechecker", "not found cached folder, creating a new one", "");
		std::error_code ec;
		if (!fs::create_directory("cached/",ec)) {
			log_err("prechecker", "can't create cached/ folder: " + ec.message(), "", true);
		}
	}
	else {
		log_info("prechecker", "found cached folder", "");
	}
}

bool extract_icon(std::string pkg) {
	std::vector<char> icon;

	std::string cmd = "tools\\scrcpy\\adb exec-out \"export CLASSPATH=/data/local/tmp/oxyconnect.jar; export ANDROID_DATA=/data/local/tmp; app_process /data/local/tmp oxyconnect " + pkg + "\"";

	HANDLE hReadPipe, hWritePipe;
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&hReadPipe, &hWritePipe, &saAttr, 0)) {
		log_err("adb subworker", "failed to open pipe", "");
		return false;
	}

	SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFOA));
	si.cb = sizeof(STARTUPINFOA);
	si.hStdOutput = hWritePipe;
	si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.dwFlags |= STARTF_USESTDHANDLES;

	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

	if (!CreateProcessA(NULL, &cmd[0], NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
		log_err("adb subworker", "failed to run adb process", "");
		CloseHandle(hReadPipe);
		CloseHandle(hWritePipe);
		return false;
	}

	CloseHandle(hWritePipe);

	char buffer[4096];
	DWORD bytesRead;
	while (ReadFile(hReadPipe, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0) {
		icon.insert(icon.end(), buffer, buffer + bytesRead);
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hReadPipe);

	std::ofstream file("cached/" + pkg + ".png", std::ios::binary);
	if (!file) { log_err("adb subworker", "cant open icon file", ""); return false; }
	file.write(icon.data(), icon.size());
	file.close();

	return true;
}

void check_apps(std::vector<std::string> apps) {
	std::unordered_set<std::string> found_apps;

	for (const auto& entry : fs::directory_iterator("cached")) {
        std::string name = entry.path().stem().string();
        std::cout << name << std::endl;
		found_apps.insert(name);
	}

	log_info("checker", "found apps: ", ""+std::to_string(found_apps.size()));
	log_info("checker", "got apps: ", "" +std::to_string(apps.size()));
	
	for (const auto& pkg : apps) {
		if (found_apps.find(pkg) == found_apps.end()) {
			log_info("checker", "haven't found "+pkg,", pulling its icon");
			extract_icon(pkg);
		}
	}
	log_info("checker", "finished sync", "");
}