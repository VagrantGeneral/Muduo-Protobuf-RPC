#include "mprpcapplication.h"

MprpcConfig MprpcApplication::m_config;

//打印参数说明
void ShowArgsHelp() {
    std::cout << "format: command -i <configfile>" << std::endl;
}

//初始化
void MprpcApplication::Init(int argc, char** argv) {
    if (argc < 2) {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    showLogo();

    int c = 0;
    std::string config_file;
    while ((c = getopt(argc, argv, "i:")) != -1) {
        switch (c) {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    //开始加载配置文件
    m_config.LoadConfigFile(config_file.c_str());

}

//获取单一对象
MprpcApplication& MprpcApplication::GetInstance() {
    static MprpcApplication app;
    return app;
}

//获取配置
MprpcConfig& MprpcApplication::GetConfig() {
    return m_config;
}

//Logo
void MprpcApplication::showLogo() {
    std::cout << "     ___           ___         ___           ___         ___                        " << std::endl;
    std::cout << "    /__/\\         /  /\\       /  /\\         /  /\\       /  /\\                  " << std::endl;
    std::cout << "   |  |::\\       /  /::\\     /  /::\\       /  /::\\     /  /:/                   " << std::endl;
    std::cout << "   |  |:|:\\     /  /:/\\:\\   /  /:/\\:\\     /  /:/\\:\\   /  /:/                 " << std::endl;
    std::cout << " __|__|:|\\:\\   /  /:/~/:/  /  /:/~/:/    /  /:/~/:/  /  /:/  ___                  " << std::endl;
    std::cout << "/__/::::| \\:\\ /__/:/ /:/  /__/:/ /:/___ /__/:/ /:/  /__/:/  /  /\\                " << std::endl;
    std::cout << "\\  \\:\\~~\\__\\/ \\  \\:\\/:/   \\  \\:\\/:::::/ \\  \\:\\/:/   \\  \\:\\ /  /:/  " << std::endl;
    std::cout << " \\  \\:\\        \\  \\::/     \\  \\::/~~~~   \\  \\::/     \\  \\:\\  /:/        " << std::endl;
    std::cout << "  \\  \\:\\        \\  \\:\\      \\  \\:\\        \\  \\:\\      \\  \\:\\/:/      " << std::endl;
    std::cout << "   \\  \\:\\        \\  \\:\\      \\  \\:\\        \\  \\:\\      \\  \\::/        " << std::endl;
    std::cout << "    \\__\\/         \\__\\/       \\__\\/         \\__\\/       \\__\\/             " << std::endl;
    std::cout << ">>>--------------------------------------------------------------<<<" << std::endl;
}