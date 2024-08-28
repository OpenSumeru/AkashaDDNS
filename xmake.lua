add_languages("cxx23")
add_requires("cpp-httplib", {configs = {ssl = true}})
add_requires("nlohmann_json")

set_policy("check.auto_map_flags", true)

target("AkashaDDNS")
    set_kind("binary")
    add_includedirs("include")
    add_files("src/*.cpp")
    set_pcxxheader("include/pch.hpp")
    add_packages("nlohmann_json", "cpp-httplib")

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