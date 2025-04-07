#pragma once
#include <unordered_map>
#include <string>

//配置文件读写类
class MprpcConfig {
public: 
    //解析加载配置文件
    void LoadConfigFile(const char* config_file);

    //查询配置信息
    std::string Load(std::string key);

private:
    //去除前后空格
    void Trim(std::string& src_buf);

    std::unordered_map<std::string, std::string> m_configMap;
    //rpcserverip rpcserverport zookeeperip zookeeperport
};