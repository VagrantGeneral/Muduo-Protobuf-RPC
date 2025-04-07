#include "mprpcconfig.h"
#include <iostream>

//解析加载配置文件
void MprpcConfig::LoadConfigFile(const char* config_file) {
    FILE* pf = fopen(config_file, "r");
    if (pf == nullptr) {
        std::cout << config_file << " is not exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    while (!feof(pf)) {
        char buff[512] = {0};
        fgets(buff, 512, pf);
        
        std::string src_buff(buff);

        Trim(src_buff); //去空格

        //判断#注释
        if(src_buff[0] == '#' || src_buff.empty()) {
            continue;
        }

        //解析
        int index = src_buff.find('=');
        if (index == -1) {
            //不合法
            continue;
        }

        std::string key;
        std::string value;
        key = src_buff.substr(0, index);
        Trim(key);
        int indexend = src_buff.find('\n', index);
        value = src_buff.substr(index+1, indexend-index-1);
        Trim(value);
        m_configMap.insert({key, value});
    }

}

//查询配置信息
std::string MprpcConfig::Load(std::string key) {
    auto iter = m_configMap.find(key);
    if (iter == m_configMap.end()) {
        return "";
    }
    return iter->second;
}

//去除前后空格
void MprpcConfig::Trim(std::string& src_buf) {
    //去掉首部空格
    int index = src_buf.find_first_not_of(' ');
    if (index != -1) {
        //前方有空格
        src_buf = src_buf.substr(index);
    }

    //去掉尾部空格
    index = src_buf.find_last_not_of(' ');
    if (index != -1) {
        //后方有空格
        src_buf = src_buf.substr(0, index+1);
    }
}

