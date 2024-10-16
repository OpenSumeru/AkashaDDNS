add_rules("mode.release", "mode.debug")

option("use_spdlog")
    set_description("Enable spdlog for logging")
    set_default(true)
    set_showmenu(true)
option_end()

if has_config("use_spdlog") then
    add_requires("spdlog >=1.14.1", {optional = false})
end

add_languages("cxx20")
add_requires("cpp-httplib", {configs = {ssl = true}})
add_requires("nlohmann_json")
add_requires("cxxopts")

target("AkashaDDNS")
    set_kind("binary")
    add_includedirs("include")
    add_files("src/*.cpp")

    if has_config("use_spdlog") then
        add_packages("spdlog")
        add_defines("USE_SPDLOG")
    else
        add_defines("USE_CUSTOM_LOGGER")
    end

    add_packages("cpp-httplib", "nlohmann_json", "cxxopts")

    if is_plat("windows") then
        add_cxxflags("/utf-8")
    end

target_end()

target("AkashaDDNS-Old")
    set_default("false")

    set_kind("binary")
    add_includedirs("old-cpp/include")
    add_files("old-cpp/src/*.cpp")
    set_pcxxheader("old-cpp/include/pch.h")
    add_packages("nlohmann_json", "cpp-httplib")

    if is_plat("windows") then
        add_cxxflags("/utf-8")
    end

target_end()