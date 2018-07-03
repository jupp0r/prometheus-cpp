cmake_minimum_required(VERSION 2.8.12.2 FATAL_ERROR)
project(prometheus-cpp)

enable_testing()

include(GNUInstallDirs)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)

if(NOT DEFINED CMAKE_CXX_STANDARD AND UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

option(BUILD_CIVETWEB "Embed Civet webserver in prometheus" ON)
enable_testing()
include(CTest)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads)

# suppress warnings
if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

if (BUILD_CIVETWEB)
	# civetweb
	set(CIVETWEB_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/3rdparty/civetweb/include)

	add_library(civetweb OBJECT
		3rdparty/civetweb/include/CivetServer.h
		3rdparty/civetweb/include/civetweb.h
		3rdparty/civetweb/src/CivetServer.cpp
		3rdparty/civetweb/src/civetweb.c
		3rdparty/civetweb/src/handle_form.inl
		3rdparty/civetweb/src/md5.inl
	)

	target_compile_definitions(civetweb PRIVATE
		USE_IPV6
		NDEBUG
		NO_CGI
		NO_CACHING
		NO_SSL
		NO_FILES
	)

	target_include_directories(civetweb PUBLIC
		${CIVETWEB_INCLUDE_DIR}
	)
endif()

# prometheus-cpp
add_subdirectory(lib)

if (BUILD_TESTING)
	find_package(GoogleBenchmark)
	find_package(Telegraf)
	# google mock
	add_library(gmock_main STATIC EXCLUDE_FROM_ALL
		3rdparty/googletest/googletest/src/gtest-all.cc
		3rdparty/googletest/googlemock/src/gmock-all.cc
		3rdparty/googletest/googlemock/src/gmock_main.cc
	)

	target_include_directories(gmock_main
		PUBLIC
			3rdparty/googletest/googletest/include
			3rdparty/googletest/googlemock/include
		PRIVATE
			3rdparty/googletest/googletest
			3rdparty/googletest/googlemock
	)

	target_link_libraries(gmock_main PRIVATE
		${CMAKE_THREAD_LIBS_INIT}
	)
	add_subdirectory(tests)
endif()

