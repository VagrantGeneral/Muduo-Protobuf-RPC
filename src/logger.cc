#include "logger.h"

//设置日志级别
void Logger::SetLogLevel(LogLevel level) {
    m_loglevel = level;
}

//写日志
void Logger::Log(std::string msg) {
    m_lckQue.Push(msg);
}

//单例
Logger& Logger::GetInstance() {
    static Logger logger;
    return logger;
}

Logger::Logger() {
    //启动写日志线程
    std::thread writeLogTask([&]() {
        for (;;) {
            //获取时间，将日志信息写入对应的文件
            time_t now = time(nullptr);
            tm* nowtm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "../log/%d-%d-%d-log.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);

            FILE* pf = fopen(file_name, "a+");
            if (pf == nullptr) {
                std::cout << "Logger file : " << file_name << " open failed!" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = m_lckQue.Pop();

            char time_buff[128] = {0};
            sprintf(time_buff, "[%5s] %d:%d:%d => ", (m_loglevel == INFO ? "INFO" : "ERROR"), nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec);
            msg.insert(0, time_buff);
            msg.append("\n");

            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });
    
    //设置分离线程
    writeLogTask.detach(); 
}



