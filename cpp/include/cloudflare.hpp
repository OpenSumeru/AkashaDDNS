#pragma once
#include "pch.h"

Json getResult(httplib::Client& client, std::string path, std::string pathHead = "");

Json putResult(httplib::Client& client, std::string path, std::string text, std::string type, std::string pathHead = "");

bool verifyAPI(httplib::Client& client, Json config);

std::string findZoneId(httplib::Client& client, Json config);

std::string findRecordId(httplib::Client& client, std::string zoneId, std::string name);

std::string putRecordId(httplib::Client& client, std::string zoneId, std::string dnsRecordId, std::string ipAddress, std::string name);