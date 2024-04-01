#include "util.h"

#include <optional>
#include <string_view>

#include "absl/status/status.h"
#include "hnswlib/hnswlib.h"
#include "re2/re2.h"
#include "sqlite3.h"
#include "vector.h"

namespace sqlite_vector {

bool IsValidColumnName(std::string_view name) {
  if (name.empty() || sqlite3_keyword_check(name.data(), name.size()) != 0) {
    return false;
  }

  static const re2::RE2 kColumnNameRegex("^[a-zA-Z_][a-zA-Z0-9_\\$]*$");

  return re2::RE2::FullMatch(name, kColumnNameRegex);
}

std::optional<std::string_view> DetectSIMD() {
#ifdef USE_SSE
  return "SSE";
#elif defined(USE_AVX)
  return "AVX";
#elif defined(USE_AVX512)
  return "AVX512";
#else
  return std::nullopt;
#endif
}

std::pair<bool, std::string_view> IsMinimumSqlite3VersionMet() {
  int version = sqlite3_libversion_number();
  // Checks whether sqlite3_vtab_in() is available.
  if (version < 3038000) {
    return {false, "sqlite version 3.38.0 or higher is required."};
  }
  return {true, ""};
}

}  // end namespace sqlite_vector