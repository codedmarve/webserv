#ifndef COLORS_H
#define COLORS_H

#include "./AllHeaders.hpp"

// Reset
const std::string RESET   = "\033[0m";
const std::string CURSIVE_GRAY ="\033[2;37m""\033[3m";

// Regular colors
const std::string BLACK   = "\033[30m";
const std::string RED     = "\033[31m";
const std::string GREEN   = "\033[32m";
const std::string YELLOW  = "\033[33m";
const std::string BLUE    = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN    = "\033[36m";
const std::string WHITE   = "\033[37m";

// Bold colors
const std::string BBLACK   = "\033[1m\033[30m";
const std::string BRED     = "\033[1m\033[31m";
const std::string BGREEN   = "\033[1m\033[32m";
const std::string BYELLOW  = "\033[1m\033[33m";
const std::string BBLUE    = "\033[1m\033[34m";
const std::string BMAGENTA = "\033[1m\033[35m";
const std::string BCYAN    = "\033[1m\033[36m";
const std::string BWHITE   = "\033[1m\033[37m";

// Underline colors
const std::string UBLACK   = "\033[4m\033[30m";
const std::string URED     = "\033[4m\033[31m";
const std::string UGREEN   = "\033[4m\033[32m";
const std::string UYELLOW  = "\033[4m\033[33m";
const std::string UBLUE    = "\033[4m\033[34m";
const std::string UMAGENTA = "\033[4m\033[35m";
const std::string UCYAN    = "\033[4m\033[36m";
const std::string UWHITE   = "\033[4m\033[37m";

// Background colors
const std::string BGBLACK   = "\033[40m";
const std::string BGRED     = "\033[41m";
const std::string BGGREEN   = "\033[42m";
const std::string BGYELLOW  = "\033[43m";
const std::string BGBLUE    = "\033[44m";
const std::string BGMAGENTA = "\033[45m";
const std::string BGCYAN    = "\033[46m";
const std::string BGWHITE   = "\033[47m";

// High intensity colors
const std::string HBLACK   = "\033[90m";
const std::string HRED     = "\033[91m";
const std::string HGREEN   = "\033[92m";
const std::string HYELLOW  = "\033[93m";
const std::string HBLUE    = "\033[94m";
const std::string HMAGENTA = "\033[95m";
const std::string HCYAN    = "\033[96m";
const std::string HWHITE   = "\033[97m";

// Bold high intensity colors
const std::string BHBLACK   = "\033[1m\033[90m";
const std::string BHRED     = "\033[1m\033[91m";
const std::string BHGREEN   = "\033[1m\033[92m";
const std::string BHYELLOW  = "\033[1m\033[93m";
const std::string BHBLUE    = "\033[1m\033[94m";
const std::string BHMAGENTA = "\033[1m\033[95m";
const std::string BHCYAN    = "\033[1m\033[96m";
const std::string BHWHITE   = "\033[1m\033[97m";

// High intensity background colors
const std::string HIBGBLACK   = "\033[0m\033[100m";
const std::string HIBGRED     = "\033[0m\033[101m";
const std::string HIBGGREEN   = "\033[0m\033[102m";
const std::string HIBGYELLOW  = "\033[0m\033[103m";
const std::string HIBGBLUE    = "\033[0m\033[104m";
const std::string HIBGMAGENTA = "\033[0m\033[105m";
const std::string HIBGCYAN    = "\033[0m\033[106m";
const std::string HIBGWHITE   = "\033[0m\033[107m";

#endif // COLORS_H
