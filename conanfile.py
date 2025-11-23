# !/usr/bin/env python
# -*- coding: utf-8 -*-

import json, os, glob
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools.env import VirtualBuildEnv
from conan.tools.files import copy

required_conan_version = ">=2.0"


class LS50RemoteConan(ConanFile):
    jsonInfo = json.load(open("info.json", 'r'))
    # ---Package reference---
    name = jsonInfo["projectName"].lower()
    version = "%u.%u.%u" % (jsonInfo["version"]["major"], jsonInfo["version"]["minor"], jsonInfo["version"]["patch"])
    user = jsonInfo["domain"]
    channel = "%s" % ("snapshot" if jsonInfo["version"]["snapshot"] else "stable")
    # ---Metadata---
    description = jsonInfo["projectDescription"]
    license = jsonInfo["license"]
    author = jsonInfo["vendor"]
    topics = jsonInfo["topics"]
    homepage = jsonInfo["repository"]
    url = jsonInfo["repository"]
    # ---Requirements---
    requires = ["qt/6.10.0@de.privatehive/stable",
                "qtappbase/1.9.0@de.privatehive/stable"]
    tool_requires = ["cmake/[>=3.22.6 <4]", "ninja/[>=1.11.1]"]
    # ---Sources---
    exports = ["info.json", "LICENSE"]
    exports_sources = ["info.json", "*.txt", "src/*"]
    # ---Binary model---
    settings = "os", "compiler", "build_type", "arch"
    options = {}
    default_options = {
        "qtappbase/*:qml": True,
        "qt/*:GUI": True,
        "qt/*:opengl": "desktop",
        "qt/*:quick2style": "basic",
        "qt/*:qtbase": True,
        "qt/*:widgets": True,
        "qt/*:qtdeclarative": True,
        "qt/*:qtsvg": True,
        "qt/*:qttools": True,
        "qt/*:qtshadertools": True,
        "qt/*:qttranslations": True,
    }
    # ---Build---
    generators = []
    # ---Folders---
    no_copy_source = False

    def generate(self):
        ms = VirtualBuildEnv(self)
        CMakeDeps(self).generate()
        tc = CMakeToolchain(self, generator="Ninja")
        qml_import_path = []
        for require, dependency in self.dependencies.items():
            path = dependency.runenv_info.vars(self, scope='run').get("QML_IMPORT_PATH")
            if path is not None:
                qml_import_path.append(path)
        tc.variables["QT_QML_OUTPUT_DIRECTORY"] = "${CMAKE_CURRENT_LIST_DIR}/src"
        qml_import_path.append("${QT_QML_OUTPUT_DIRECTORY}")
        tc.variables["QML_IMPORT_PATH"] = ";".join(qml_import_path)
        tc.generate()
        ms.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def deploy(self):
        copy(self, "LS50Remote*", src=self.package_folder, dst=self.deploy_folder)
