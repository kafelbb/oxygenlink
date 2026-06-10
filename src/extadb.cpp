#pragma once

#include "extadb.h"

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <array>
#include <thread> 
#include <fstream>
#include <filesystem>

#include "logger.h"

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

std::string exec_com(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;

    #ifdef _WIN32
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
    #else
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    #endif

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

std::vector<std::string> get_installed_pkgs() {
    std::vector<std::string> packages;
    std::string output = exec_com("tools\\scrcpy\\adb shell pm list packages -3");

    std::size_t pos = 0;
    while ((pos = output.find("package:", pos)) != std::string::npos) {
        pos += 8;
        std::size_t end = output.find("\n", pos);
        if (end == std::string::npos) break;

        std::string pkg = output.substr(pos, end - pos);
        if (!pkg.empty() && pkg.back() == '\r') pkg.pop_back();

        packages.push_back(pkg);
        pos = end + 1;
    }
    return packages;
}


std::string get_apk_path(const std::string& packageName) {
    std::string output = exec_com("tools\\scrcpy\\adb shell pm path " + packageName);

    if (output.find("package:") != std::string::npos) {
        output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());

        size_t first_line_end = output.find('\n');
        std::string first_line = (first_line_end != std::string::npos) ? output.substr(0, first_line_end) : output;
        if (first_line.find("package:") == 0) {
            return first_line.substr(8);
        }
    }
    return "";
}

void launch_app(std::string pkg) {
    std::thread t([pkg]() {
        std::string cmd = "tools\\scrcpy\\scrcpy --new-display=/192 -x --start-app=" + pkg + " --keep-active --no-vd-system-decorations";
        exec_com(cmd);
        });
    t.detach();
}