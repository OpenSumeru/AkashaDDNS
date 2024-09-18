#ifndef RECORD_INFO
#define RECORD_INFO
#include <optional>
#include <string>

#include "pch.hpp"

struct RecordInfo
{
    std::optional<std::string> recordName;
    std::optional<std::string> recordId;
    std::optional<std::string> zoneName;
    std::optional<std::string> zoneId;
};

#endif