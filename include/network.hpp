#pragma once
#include "pch.h"

inline std::string getIPv4()
{
    static httplib::Client client("https://api.ipify.org");
    auto result = safeGet(client, "/");
    if (!result.has_value())
    {
        return "";
    }
    return std::string(result.value()->body);
}