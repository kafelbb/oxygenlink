#ifndef GE_LOG
#define GE_LOG

#include "pfd.h"

#include <iostream>

inline void log_err(std::string title, std::string str1, std::string str2, bool display = false) {
	std::string errstr = str1 + str2;
	std::cout << "[ INFO - " << title << "] " << errstr << std::endl;
	if (display) {
		pfd::message(title, errstr, pfd::choice::ok, pfd::icon::info);
	}
}

inline void log_info(std::string title, std::string str1, std::string str2, bool display=false) {
	std::string errstr = str1 + str2;
	std::cout << "[ INFO - " << title << "] " << errstr << std::endl;
	if (display) {
		pfd::message(title, errstr, pfd::choice::ok, pfd::icon::info);
	}
}

inline void log_warn(std::string title, std::string str1, std::string str2, bool display = false) {
	std::string errstr = str1 + str2;
	std::cout << "[ INFO - " << title << "] " << errstr << std::endl;
	if (display) {
		pfd::message(title, errstr, pfd::choice::ok, pfd::icon::info);
	}
}
#endif