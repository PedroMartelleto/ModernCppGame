#pragma once

#include "Commons.h"

/// <summary>
/// The logger verbose
/// -1 = Nothing.
///  0 = Msgs only.
///  1 = Msgs + errors.
///  2 = Msgs + errors + warnings.
///  3 = Msgs + errors + warnings + debug.
/// </summary>
#define LOGGER_VERBOSE 3

#define LOGGER_MESSAGE(a) \
    do { if (LOGGER_VERBOSE >= 0) std::cout << "[MSG]: " << (a) << "\n"; } while(false)

#define LOGGER_ERROR(a) \
    do { if (LOGGER_VERBOSE >= 1) std::cerr << "[ERR]: " << (a) << "\n"; } while(false)

#define LOGGER_WARNING(a) \
    do { if (LOGGER_VERBOSE >= 2) std::cout << "[WRN]: " << (a) << "\n"; } while(false)

#define LOGGER_DEBUG(a) \
    do { if (LOGGER_VERBOSE >= 3) std::cout << "[DBG]: " << (a) << "\n"; } while(false)

#define LOGGER_VAR(a) \
    do { if (LOGGER_VERBOSE >= 3) std::cout << "[DBG]: " << #a " = " << (a) << "\n"; } while(false)

