#pragma once

#if defined(_WIN32)
#  ifdef PROMETHEUS_CPP_COMPILE_PULL
#    define PROMETHEUS_CPP_PULL_EXPORT __declspec(dllexport)
#  else
#    define PROMETHEUS_CPP_PULL_EXPORT __declspec(dllimport)
#  endif
#else
#  define PROMETHEUS_CPP_PULL_EXPORT __attribute__((visibility("default")))
#endif
