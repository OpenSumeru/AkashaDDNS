add_languages("cxx20")
add_requires("nlohmann_json", "openssl3", "cpp-httplib")

target("AkashaDDNS")
    add_includedirs("include")
    add_files("src/*.cpp")
    add_headerfiles("include/*.h", "include/*.hpp")
    add_packages("nlohmann_json", "openssl3", "cpp-httplib")
target_end()