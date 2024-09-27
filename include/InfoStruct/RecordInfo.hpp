#ifndef RECORD_INFO_HPP
#define RECORD_INFO_HPP
#include <optional>
#include <string>

struct RecordInfo
{
    std::optional<std::string> recordName;
    std::optional<std::string> recordId;
    std::optional<std::string> zoneName;
    std::optional<std::string> zoneId;
};

#endif