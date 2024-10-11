add_rules("mode.release", "mode.debug")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "./"})

add_languages("cxx20")
add_requires("cpp-httplib", {configs = {ssl = true}})
add_requires("nlohmann_json")
add_requires("fmt")

target("AkashaDDNS")
    set_kind("binary")
    add_includedirs("include")
    add_files("src/*.cpp")
    add_files("src/InfoStruct/*.cpp")
    add_files("src/ReuseFn/*.cpp")
    add_files("src/StructUsage/*.cpp")
    set_pcxxheader("include/pch.hpp")
    add_packages("nlohmann_json", "cpp-httplib","fmt")

    add_cxxflags("/utf-8")

target_end()

target("AkashaDDNS-Old")
    set_kind("binary")
    add_includedirs("old-cpp/include")
    add_files("old-cpp/src/*.cpp")
    set_pcxxheader("old-cpp/include/pch.h")
    add_packages("nlohmann_json", "cpp-httplib")

    add_cxxflags("/utf-8")

target_end()