#pragma once
#include "mprpcconfig.h"
#include <unistd.h>
#include <iostream>
#include <string>

//rpc框架的基础类------单例模式
class MprpcApplication {

public:
    //初始化
    static void Init(int argc, char** argv);

    //获取单一对象
    static MprpcApplication& GetInstance();

    //获取配置
    static MprpcConfig& GetConfig();

    //展示Logo
    static void showLogo();

private:
    static MprpcConfig m_config;

    MprpcApplication() {}
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(const MprpcApplication&&) = delete;
};