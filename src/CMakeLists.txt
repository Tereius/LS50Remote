find_package(Qt5 COMPONENTS Core Qml Quick QuickControls2 Widgets Network REQUIRED)

set(APP_PORTABLE_MODE ON CACHE BOOL "Install app in portable mode")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(qt_rcc_resources "${PROJECT_SOURCE_DIR}/res/qt_resources.qrc;${PROJECT_SOURCE_DIR}/res/default_icons.qrc")
set(win_resources "${PROJECT_SOURCE_DIR}/res/win_resources.rc")
source_group("Resource Files" FILES "${qt_rcc_resources}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

if(WIN32)
	add_definitions(-DNOMINMAX)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

if(APP_PORTABLE_MODE)
	add_definitions(-DPORTABLE_MODE)
endif()

# -------------------------------App-------------------------------
set(src_files
	main.cpp
	Networking.cpp
	KefDevice.cpp
)

# private header
set(header_files
	Networking.h
	KefDevice.h
)

qt5_add_resources(resSources ${qt_rcc_resources})
add_executable(${PROJECT_NAME} WIN32 ${src_files} ${header_files} ${resSources} ${win_resources})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Qml Qt5::Quick Qt5::QuickControls2 Qt5::Widgets Qt5::Network)

# add the install target
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include)

include(Windeployqt)
windeployqt(${PROJECT_NAME} "bin")

# Workaround: windeployqt is missing Qt.labs modules
install(DIRECTORY "${qml_dir}/Qt/labs/settings" DESTINATION "bin/Qt/labs")
install(DIRECTORY "${qml_dir}/QtQuick/Layouts" DESTINATION "bin/QtQuick")
