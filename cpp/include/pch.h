#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <iterator>
#include <memory>
#include <chrono>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
using StatusCode = httplib::StatusCode;

#include "json11.hpp"
using namespace json11;

#ifndef PCH_H
#define PCH_H

std::string readFileToString(const std::string& filePath);

Json readFileToJson(const std::string& filePath);

httplib::Result safeGet(httplib::Client& client, std::string path, std::vector<int> accept = {StatusCode::OK_200});

httplib::Result safePut(httplib::Client& client, std::string path, std::string text, std::string type, std::vector<int> accept = {StatusCode::OK_200});

#endif