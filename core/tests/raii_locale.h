#pragma once

#include <locale>

class RAIILocale {
 public:
  RAIILocale(const char* name) : savedLocale_(std::locale::classic()) {
    std::locale::global(std::locale(name));
  }

  ~RAIILocale() { std::locale::global(savedLocale_); }

  RAIILocale(const RAIILocale&) = delete;
  RAIILocale(RAIILocale&&) = delete;
  RAIILocale& operator=(const RAIILocale&) = delete;
  RAIILocale& operator=(RAIILocale&&) = delete;

 private:
  const std::locale savedLocale_;
};
