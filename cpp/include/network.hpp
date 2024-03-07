#pragma once
#include "pch.h"

inline std::string getIPv4()
{
    static httplib::Client client("https://api.ipify.org");
    auto ip = client.Get("/")->body;
    return ip;
}