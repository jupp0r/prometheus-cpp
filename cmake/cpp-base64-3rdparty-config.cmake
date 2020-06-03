get_filename_component(_IMPORT_PREFIX "${PROJECT_SOURCE_DIR}/3rdparty/cpp-base64/" ABSOLUTE)

add_library(base64 OBJECT
        ${_IMPORT_PREFIX}/base64.h
        ${_IMPORT_PREFIX}/base64.cpp
        )

set(BASE64_INCLUDE_DIRS ${_IMPORT_PREFIX})
