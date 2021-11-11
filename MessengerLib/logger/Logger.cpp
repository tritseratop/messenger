#include "Logger.h"
#include <iomanip> 

using namespace logger;

FileLogger::FileLogger(const std::string& info, const char* fname = "logger.txt", unsigned max_lines_count = 2)
    : numWarnings(0U)
    , numErrors(0U)
    , max_lines(max_lines_count)
    , name(fname)
{
    time(&_now);
    myFile.open(fname, std::ofstream::out | std::ofstream::trunc);

    // Write the first lines
    if (myFile.is_open()) {
        *this << e_logType::LOG_INFO << "Log file created";
        *this << e_logType::LOG_INFO << "Log file info: " + info;
    } // if
    _start = myFile.tellp();
}

FileLogger::~FileLogger() {
    if (myFile.is_open()) {
        myFile << std::endl << std::endl;

        // Report number of errors and warnings
        myFile << numWarnings << " warnings" << std::endl;
        myFile << numErrors << " errors" << std::endl;

        myFile.close();
    } // if
}

void FileLogger::writeTime(const std::string& logType) {
    std::string msg = std::to_string(lines) + "\t" + PrepTime() + logType;
    if (content.size() <= max_lines) {
        std::lock_guard<std::mutex> guard(m_writeFile);
        content.push_back(msg);
        myFile << msg;
    }
    else {
        std::lock_guard<std::mutex> guard(m_writeFile);
        content.pop_front();
        content.push_back(msg);
        myFile << msg;
        myFile.close();
        myFile.open(name, std::ofstream::out | std::ofstream::trunc);
        // all but last with \n
        for (const auto& str : content) {
            myFile << str;
        }
    }
}

std::string FileLogger::PrepTime() {
    time(&_now);
    struct tm* t;
    t = localtime(&_now);
    std::string s, m, h, D, M, Y;
    s = std::to_string(t->tm_sec);
    m = std::to_string(t->tm_min);
    h = std::to_string(t->tm_hour);
    D = std::to_string(t->tm_mday);
    M = std::to_string(t->tm_mon + 1);
    Y = std::to_string(t->tm_year + 1900);
    if (t->tm_sec < 10) s = "0" + s;
    if (t->tm_min < 10) m = "0" + m;
    if (t->tm_hour < 10) h = "0" + h;
    if (t->tm_mday < 10) D = "0" + D;
    if (t->tm_mon + 1 < 10) M = "0" + M;

    std::string res =
        "[" + Y + "-" + M + "-" + D + "T" + h + ":" + m + ":" + s + "] ";

    return res;
}

FileLogger* FileLogger::fileLogger_ = nullptr;

std::mutex FileLogger::m_genInstance;

FileLogger* FileLogger::getInstance(unsigned max_lines_count, const std::string& info, const char* fname) {
    {
        std::lock_guard<std::mutex> guard(FileLogger::m_genInstance);
        if (fileLogger_ == nullptr) {
            fileLogger_ = new FileLogger(info, fname, max_lines_count);
        }
    }
    return fileLogger_;
}