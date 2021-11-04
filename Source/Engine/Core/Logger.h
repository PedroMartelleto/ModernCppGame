#pragma once

#include "../Commons.h"

#define LOG_ERROR "ERR"
#define LOG_MSG "MSG"
#define LOG_ERROR "ERR"
#define LOG_WARNING "WRN"

#define LOGGER_DEST stderr

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif

#define DEBUG_LOG(category, level, message, ...) \
	do {  fprintf(LOGGER_DEST, "[%s %s] (%s:%d):  ", category, level, __FILENAME__, __LINE__); fprintf(LOGGER_DEST, message, ##__VA_ARGS__); fprintf(LOGGER_DEST, "\n"); } while (false)

#define LOGGER_ENUM(a) LOGGER_VAR(magic_enum::enum_name(a))

#define LOGGER_VAR(a) \
    do { std::cout << "[DBG]: " << #a " = " << (a) << "\n"; } while (false)