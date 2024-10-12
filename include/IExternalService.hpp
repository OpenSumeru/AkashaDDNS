#ifndef IEXTERNAL_SERVICE_HPP
#define IEXTERNAL_SERVICE_HPP

#include <string>

class IExternalService
{
  public:
    virtual ~IExternalService() = default;
    virtual std::string getPublicIP() = 0;
    virtual std::string getZoneID(const std::string &domain) = 0;
    virtual std::string getRecordID(const std::string &zoneID, const std::string &recordName) = 0;
    virtual bool updateDNSRecord(const std::string &zoneID, const std::string &recordID, const std::string &recordName,
                                 const std::string &ip) = 0;
    virtual std::string getDNSRecordContent(const std::string &zoneID, const std::string &recordID) = 0; // 新增的函数
};

#endif