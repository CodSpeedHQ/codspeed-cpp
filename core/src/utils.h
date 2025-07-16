#ifndef CODSPEED_UTILS_H
#define CODSPEED_UTILS_H

#include <string>

namespace codspeed {

// Cross-platform getenv wrapper
std::string safe_getenv(const char* var_name);

}  // namespace codspeed

#endif  // CODSPEED_UTILS_H
