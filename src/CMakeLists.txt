find_package(QtAppBase REQUIRED)
include(AppImage)
include(MaterialIcons)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Qml Quick QuickControls2 Svg LinguistTools Widgets)

# Download Roboto fonts
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fonts/Roboto.ttf)
    file(DOWNLOAD "https://github.com/google/fonts/raw/refs/heads/main/ofl/roboto/Roboto%5Bwdth,wght%5D.ttf" ${CMAKE_CURRENT_SOURCE_DIR}/fonts/Roboto.ttf)
endif ()

qt_add_executable(LS50Remote
        main.cpp
        Networking.h
        Networking.cpp
        KefDevice.h
        KefDevice.cpp
)

set_source_files_properties(${CMAKE_SOURCE_DIR}/resources/icons/LS50Remote_mono.svg PROPERTIES
        QT_RESOURCE_ALIAS images/LS50Remote_mono.svg
)

qt_add_qml_module(
        LS50Remote

        URI
        com.kef

        QML_FILES
        qml/main.qml
        qml/settings.qml
        qml/control.qml

        RESOURCES
        fonts/Roboto.ttf
        ${CMAKE_SOURCE_DIR}/resources/icons/LS50Remote_mono.svg
)

target_link_libraries(LS50Remote PRIVATE QtAppBase PRIVATE Qt6::Core PRIVATE Qt6::Gui PRIVATE Qt6::Network PRIVATE Qt6::Qml PRIVATE Qt6::Quick PRIVATE Qt6::QuickControls2 PRIVATE Qt6::Widgets PRIVATE Qt6::Svg)

add_material_icon_theme_resource(LS50Remote THEME_NAME material ICONS
        bluetooth
        chevron-left
        pause
        power
        alert
        content-save
        cog-outline
        volume-off
        volume-high
        wifi
)

qt_add_translations(LS50Remote TS_FILES
        "${CMAKE_SOURCE_DIR}/resources/i18n/LS50Remote_en.ts"
        "${CMAKE_SOURCE_DIR}/resources/i18n/LS50Remote_de.ts"
)

register_icon(LS50Remote
        ICO "${CMAKE_SOURCE_DIR}/resources/icons/LS50Remote.ico"
        ICNS "${CMAKE_SOURCE_DIR}/resources/icons/LS50Remote.icns"
        SCALABLE "${CMAKE_SOURCE_DIR}/resources/icons/LS50Remote.svg"
        512x512 "${CMAKE_SOURCE_DIR}/resources/icons/LS50Remote_512.png"
        256x256 "${CMAKE_SOURCE_DIR}/resources/icons/LS50Remote_256.png"
        128x128 "${CMAKE_SOURCE_DIR}/resources/icons/LS50Remote_128.png"
        64x64 "${CMAKE_SOURCE_DIR}/resources/icons/LS50Remote_64.png"
        48x48 "${CMAKE_SOURCE_DIR}/resources/icons/LS50Remote_48.png"
        32x32 "${CMAKE_SOURCE_DIR}/resources/icons/LS50Remote_32.png"
)

install_app(LS50Remote)
