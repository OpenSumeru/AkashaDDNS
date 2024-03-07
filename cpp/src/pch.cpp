#include "pch.h"

std::string readFileToString(const std::string& filePath) 
{
    // 打开文件
    std::ifstream file(filePath);

    // 检查文件是否成功打开
    if (!file.is_open()) {
        std::cout << "Error opening file: " << filePath << std::endl;
        return "";  // 返回空字符串表示出错
    }

    // 使用流迭代器将文件内容读取到字符串
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // 关闭文件
    file.close();

    return content;
}

Json readFileToJson(const std::string& filePath)
{
    std::string error;
    auto result = Json::parse(readFileToString(filePath),error);
    std::cout << error << std::endl;
    return result;
}

httplib::Result safeGet(httplib::Client& client, std::string path, std::vector<int> accept)
{
    auto result = client.Get(path);
    
    for (auto rax : accept)
    {
        if (result->status == rax)
        {
            return result;
        }
    }

    std::cout << "Error: " << result->reason << "\nError Message: " << result->body << std::endl;
    return httplib::Result();
}

httplib::Result safePut(httplib::Client& client, std::string path, std::string text, std::string type, std::vector<int> accept)
{
    auto result = client.Put(path, text, type);
    
    for (auto rax : accept)
    {
        if (result->status == rax)
        {
            return result;
        }
    }

    std::cout << "Error: " << result->reason << "\nError Message: " << result->body << std::endl;
    return httplib::Result();
}