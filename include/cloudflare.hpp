#pragma once
#include "pch.h"

json getResult(httplib::Client& client, std::string path, std::string pathHead = "");

json putResult(httplib::Client& client, std::string path, std::string text, std::string type, std::string pathHead = "");

bool verifyAPI(httplib::Client& client, json config);

std::string findZoneId(httplib::Client& client, json config);

std::string findRecordId(httplib::Client& client, std::string zoneId, std::string name);

std::string putRecordId(httplib::Client& client, std::string zoneId, std::string dnsRecordId, std::string ipAddress, std::string name);