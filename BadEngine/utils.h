#pragma once

#include <string>

namespace utility
{
  void dbg_print(const std::string &msg);
  int r_exit(int code, const std::string &msg = "");
  std::string read_file(const char *file_path);
  static inline constexpr double PI = 3.14159265359;
};

