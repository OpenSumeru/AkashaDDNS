#pragma once
#include "pch.h"

bool isIPv4Valid(std::string &ip);
bool isIPv6Valid(std::string &ip);

json getIPv4();
json getIPv6();