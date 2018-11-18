import json, os
from conans import ConanFile, CMake, tools


class LS50RemoteConan(ConanFile):
    jsonInfo = json.loads(tools.load("info.json"))
    name = jsonInfo["projectName"]
    version = "%u.%u.%u" % (jsonInfo["version"]["major"], jsonInfo["version"]["minor"], jsonInfo["version"]["patch"])
    license = jsonInfo["license"]
    url = jsonInfo["repository"]
    description = jsonInfo["projectDescription"]
    author = jsonInfo["vendor"]
    homepage = jsonInfo["repository"]
    requires = "Qt/[>=5.10]@tereius/stable"
    settings = "os", "compiler", "build_type", "arch"
    options = {"portable": [True, False]}
    default_options = "portable=True", "Qt:shared=True", "Qt:qtbase=True", "Qt:qttranslations=True", "Qt:qttools=True", "Qt:qtsvg=True", "Qt:qtdeclarative=True", "Qt:qtgraphicaleffects=True", "Qt:qtquickcontrols2=True"
    generators = "cmake"
    exports = "info.json"
    exports_sources = "*"

    def configure_cmake(self):
        cmake = CMake(self)
        if self.options.portable:
            cmake.definitions["APP_PORTABLE_MODE"] = True
        else:
            cmake.definitions["APP_PORTABLE_MODE"] = False
        cmake.configure()
        return cmake

    def build(self):
        tools.replace_in_file(os.path.join(self.build_folder, "CMakeLists.txt"), "### CONAN_BEACON ###", 'include(%s)\n%s' % (os.path.join(self.install_folder, "conanbuildinfo.cmake").replace("\\", "/"), "conan_basic_setup()"), strict=False)
        cmake = self.configure_cmake()
        cmake.build()

    def package(self):
        cmake = self.configure_cmake()
        cmake.install()
