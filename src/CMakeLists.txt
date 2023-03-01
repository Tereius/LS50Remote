find_package(QtDeployHelper REQUIRED NO_MODULE)
include(QtDeployCommon)
include(AddMaterialIcons)
include(Linuxdeployqt)

qt_add_executable(${PROJECT_NAME}
									main.cpp
									AdvancedQmlApplicationEngine.h
									AdvancedQmlApplicationEngine.cpp
									Networking.h
									Networking.cpp
									KefDevice.h
									KefDevice.cpp
									${${PROJECT_NAME}_ICONS}
									)

qt_add_qml_module(${PROJECT_NAME}  URI Application  VERSION 1.0 RESOURCE_PREFIX / QML_FILES
									qml/main.qml
									qml/settings.qml
									qml/control.qml
									)

target_compile_definitions(${PROJECT_NAME} PRIVATE QML_DIR="${CMAKE_CURRENT_SOURCE_DIR}/qml")

if(APP_PORTABLE_MODE)
	target_compile_definitions(${PROJECT_NAME} PRIVATE -DPORTABLE_MODE)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core PRIVATE Qt6::Gui PRIVATE Qt6::Widgets PRIVATE Qt6::Qml PRIVATE Qt6::QuickControls2)

add_material_icon_theme_resource(${PROJECT_NAME} THEME_NAME material ICONS
																 bluetooth
																 chevron_left
																 pause
																 play_arrow
																 power_settings_new
																 report_problem
																 save
																 settings
																 volume_off
																 volume_up
																 wifi
																 )

qt_add_resources(${PROJECT_NAME} images PREFIX / BASE images FILES
								 images/ic_settings_remote_black.png
								 images/ic_settings_remote_white.png
								 )

qt_add_translations(${PROJECT_NAME} TS_FILES
										i18n/LS50Remote_de_DE.ts
										)

install(TARGETS ${PROJECT_NAME}
				BUNDLE  DESTINATION .
				RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
				)

linuxdeployqt(${PROJECT_NAME})

#qt_generate_deploy_qml_app_script(
#				TARGET ${PROJECT_NAME}
#				FILENAME_VARIABLE deploy_script
#				NO_UNSUPPORTED_PLATFORM_ERROR
#				DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM
#)

#install(SCRIPT ${deploy_script})
