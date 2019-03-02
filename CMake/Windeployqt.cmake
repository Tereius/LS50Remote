# The MIT License (MIT)
#
# Copyright (c) 2017 Nathan Osman, Björn Stresing
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

find_package(Qt5Core REQUIRED)

# Retrieve the absolute path to qmake and then use that path to find
# the windeployqt binary
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
find_path(qml_dir NAMES builtins.qmltypes HINTS "${_qt_bin_dir}/../qml")

# Running this with MSVC 2015 requires CMake 3.6+
if((MSVC_VERSION VERSION_EQUAL 1900 OR MSVC_VERSION VERSION_GREATER 1900)
        AND CMAKE_VERSION VERSION_LESS "3.6")
    message(WARNING "Deploying with MSVC 2015+ requires CMake 3.6+")
endif()

# Add commands that copy the Qt runtime to the target's output directory after
# build and install the Qt runtime to the specified directory
function(windeployqt target directory)
    
    # install(CODE ...) doesn't support generator expressions, but
    # file(GENERATE ...) does - store the path in a file
    file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${target}_path"
        CONTENT "$<TARGET_FILE:${target}>"
    )

    # Before installation, run a series of commands that copy each of the Qt
    # runtime files to the appropriate directory for installation
    install(CODE
        "
        file(READ \"${CMAKE_CURRENT_BINARY_DIR}/${target}_path\" _file)
        if(NOT EXISTS \"${qml_dir}/QtQuick/PrivateWidgets\")
            message(WARNING \"Workaround for bug: windeployqt is missing PrivateWidgets. Generating empty folder.\")
            file(MAKE_DIRECTORY \"${qml_dir}/QtQuick/PrivateWidgets\")
        endif()
        execute_process(
            COMMAND \"${CMAKE_COMMAND}\" -E
                env PATH=\"${_qt_bin_dir}\" \"${WINDEPLOYQT_EXECUTABLE}\"
                    --dry-run
                    --qmldir \"${qml_dir}\"
                    --no-compiler-runtime
                    --no-angle
                    --no-opengl-sw
                    --list mapping
                    \${_file}
            OUTPUT_VARIABLE _output
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        separate_arguments(_files WINDOWS_COMMAND \${_output})
        while(_files)
            list(GET _files 0 _src)
            list(GET _files 1 _dest)
            execute_process(
                COMMAND \"${CMAKE_COMMAND}\" -E
                    copy \${_src} \"\${CMAKE_INSTALL_PREFIX}/${directory}/\${_dest}\"
            )
            list(REMOVE_AT _files 0 1)
        endwhile()
        "
    )

    get_target_property(targetName ${target} NAME)
    get_target_property(linked_libs ${ARGV0} INTERFACE_LINK_LIBRARIES)

    set(dirs)
    # Add the dependencies of the target to the fixup search path and to the linker search path
    foreach(liked_lib ${linked_libs})
        if(TARGET ${liked_lib})
            get_target_property(lib_path ${liked_lib} LOCATION)
            get_filename_component(lib_dir ${lib_path} DIRECTORY)
            list(APPEND dirs "${lib_dir}/../lib")
            list(APPEND dirs "${lib_dir}/../bin")
        else()
            get_filename_component(lib_dir ${liked_lib} DIRECTORY)
            list(APPEND dirs "${lib_dir}/../lib")
            list(APPEND dirs "${lib_dir}/../bin")
        endif()
    endforeach()

    # Add CMAKE_LIBRARY_PATH to the fixup search path (set by Conan).
    foreach(other_lib ${CMAKE_LIBRARY_PATH})
        get_filename_component(lib_dir ${other_lib} DIRECTORY)
        list(APPEND dirs "${lib_dir}/lib")
        list(APPEND dirs "${lib_dir}/bin")
    endforeach()

    if(MSVC)
        # Append all linker search paths to the PATH env variable for debugging
        set_target_properties(${target} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=${dirs};%PATH%")
    endif()

    link_directories("${dirs}")

    # Run cmakes fixup_bundle during installation
    install(CODE
        "
        file(READ \"${CMAKE_CURRENT_BINARY_DIR}/${target}_path\" _file)
        include(BundleUtilities)
        fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${directory}/${targetName}${CMAKE_EXECUTABLE_SUFFIX}\" \"\" \"${dirs}\")
        "
    )

    # Write a qt.conf file overwriting the Qt prefix path
    install(CODE "file(WRITE \"${CMAKE_INSTALL_PREFIX}/${directory}/qt.conf\" \"[PATHS]\nPrefix = .\")")

endfunction()

mark_as_advanced(WINDEPLOYQT_EXECUTABLE)
