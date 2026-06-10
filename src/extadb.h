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

std::string exec_com(const std::string& cmd);
std::vector<std::string> get_installed_pkgs();
std::string get_apk_path(const std::string& pkgname);

void launch_app(std::string pkg);