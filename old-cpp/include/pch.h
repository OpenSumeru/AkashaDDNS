#ifndef PCH_H
#define PCH_H

#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
using StatusCode = httplib::StatusCode;

#include <nlohmann/json.hpp>
using json = nlohmann::json;

std::string readFileToString(const std::string &filePath);

json readFileTojson(const std::string &filePath);

std::optional<httplib::Result> safeGet(httplib::Client &client, std::string path,
                                       std::vector<int> accept = {StatusCode::OK_200});

std::optional<httplib::Result> safePut(httplib::Client &client, std::string path, std::string text, std::string type,
                                       std::vector<int> accept = {StatusCode::OK_200});

#endif