#pragma once
#include "pch.h"

inline std::string getIPv4()
{
    static httplib::Client client("https://api.ipify.org");
    auto&& result = safeGet(client, "/");
    if (result->status != StatusCode::OK_200)
    {
        return "";
    }
    return std::string(result->body);
}