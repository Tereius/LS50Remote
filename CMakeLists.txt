cmake_minimum_required(VERSION 3.21)
# Set a default search path for CMake modules
set(CMAKE_MODULE_PATH  "${CMAKE_SOURCE_DIR}/CMake" "${CMAKE_MODULE_PATH}")
include(JSONParser)

# Parse the info.json and write a info.h file
file(READ info.json jsonInfo)
sbeParseJson(info jsonInfo)

project(${info.projectName} VERSION "${info.version.major}.${info.version.minor}.${info.version.patch}" LANGUAGES CXX)

#set(CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR} ${CMAKE_PREFIX_PATH})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if we use conan
#if(EXISTS "${PROJECT_BINARY_DIR}/conanbuildinfo.cmake")
#	include("${PROJECT_BINARY_DIR}/conanbuildinfo.cmake")
#	conan_basic_setup()
#elseif(EXISTS "${PROJECT_SOURCE_DIR}/conanbuildinfo.cmake")
#	include("${PROJECT_SOURCE_DIR}/conanbuildinfo.cmake")
#	conan_basic_setup()
#endif()

# Configure a header file to pass some of the CMake settings to the source code.
include_directories("${PROJECT_BINARY_DIR}/src")
file(WRITE "${PROJECT_BINARY_DIR}/src/info.h" "")
foreach(var ${info})
	string(TOUPPER "${var}" UPPER_VAR)
	string(REPLACE "." "_" UPPER_VAR "${UPPER_VAR}")
	if (${${var}} MATCHES "^[0-9]+$")
		file(APPEND "${PROJECT_BINARY_DIR}/src/info.h" "#define ${UPPER_VAR} ${${var}}\n")
	else()
		file(APPEND "${PROJECT_BINARY_DIR}/src/info.h" "#define ${UPPER_VAR} \"${${var}}\"\n")
	endif()
endforeach()

include(FeatureSummary)
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Qml Quick QuickControls2 Linguist)

#find_package(Qt6Widgets REQUIRED NO_MODULE)
#find_package(Qt6Gui REQUIRED NO_MODULE)
#find_package(Qt6QSvgPlugin REQUIRED NO_MODULE PATH_SUFFIXES lib/cmake/Qt6Gui)
#find_package(Qt6QSvgIconPlugin REQUIRED NO_MODULE PATH_SUFFIXES lib/cmake/Qt6Gui)
#find_package(Qt6Xml REQUIRED NO_MODULE)
#find_package(Qt6Svg REQUIRED NO_MODULE)
#find_package(Qt6Qml REQUIRED NO_MODULE)
#find_package(Qt6QmlWorkerScript REQUIRED NO_MODULE)
#find_package(Qt6Quick REQUIRED NO_MODULE)
#find_package(Qt6QuickControls2 REQUIRED NO_MODULE)
#find_package(Qt6Core5Compat REQUIRED NO_MODULE)
#find_package(Qt6SvgWidgets REQUIRED NO_MODULE)

#find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)
#find_package(Qt6 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

# Handle CPack stuff
set(CPACK_PACKAGE_NAME "${info.projectName}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${info.projectDescription}")
set(CPACK_PACKAGE_VENDOR "${info.vendor}")
set(CPACK_PACKAGE_CONTACT "${info.contact}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR ${info.version.major})
set(CPACK_PACKAGE_VERSION_MINOR ${info.version.minor})
set(CPACK_PACKAGE_VERSION_PATCH ${info.version.patch})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_PACKAGE_FILE_NAME  "${PROJECT_NAME}")
set(CPACK_PACKAGE_EXECUTABLES "${PROJECT_NAME};${PROJECT_NAME}")

set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\bin\\\\${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}\\\" -u'")
include(CPack)
