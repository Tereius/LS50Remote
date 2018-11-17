find_package(Qt5 COMPONENTS Core Svg Quick Concurrent QuickControls2 REQUIRED)
find_path(Qt5_qml_DIR NAMES QtQml/qmldir PATH_SUFFIXES "lib" "lib/qml" "qml")
find_path(Qt5_plugin_DIR NAMES platforms/qminimald.dll platforms/qminimal.dll PATH_SUFFIXES "plugins")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(qt_rcc_resources "${PROJECT_SOURCE_DIR}/res/qt_resources.qrc;${PROJECT_SOURCE_DIR}/res/default_icons.qrc")

set(QML2_IMPORT_PATH "${EXTERNAL_QML2_PATH}")
set(QML_IMPORT_PATH "${EXTERNAL_QML2_PATH}")
set(QT5_ROOT_PATH "${Qt5_DIR}/../../..")

include_directories("${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

if(MSVC)
	add_definitions(-DNOMINMAX)
	add_definitions(-DQT_QML_DEBUG)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-DQML_IMPORT_PATH="${Qt5_qml_DIR}")
	add_definitions(-DQT_PLUGIN_PATH="${Qt5_plugin_DIR}")
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

source_group("Resource Files" FILES ${qt_rcc_resources})

qt5_add_resources(resSources ${qt_rcc_resources})
add_executable(${PROJECT_NAME} WIN32 ${src_files} ${header_files} ${resSources})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Svg Qt5::Quick Qt5::Concurrent Qt5::QuickControls2)

# add the install target
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include)

include(Windeployqt)
windeployqt(${PROJECT_NAME} "bin")
