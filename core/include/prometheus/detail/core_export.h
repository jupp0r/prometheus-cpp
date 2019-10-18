#pragma once

#if defined(_WIN32)
#ifdef PROMETHEUS_CPP_COMPILE_CORE
#define PROMETHEUS_CPP_CORE_EXPORT __declspec(dllexport)
#else
#define PROMETHEUS_CPP_CORE_EXPORT __declspec(dllimport)
#endif
#else
#define PROMETHEUS_CPP_CORE_EXPORT __attribute__((visibility("default")))
#endif
