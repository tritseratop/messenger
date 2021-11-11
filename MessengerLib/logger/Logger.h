#ifndef FILELOGGER_HPP
#define FILELOGGER_HPP

#include <fstream>
#include <deque>
#include <string>
#include <chrono>
#include <mutex>
#include <atomic>

namespace logger {


    class FileLogger {

    protected:
        static FileLogger* fileLogger_;
        static std::mutex m_genInstance;

        // ctor
        explicit FileLogger(const std::string& info, const char* fname, unsigned max_lines_count);

        // dtor
        ~FileLogger();

    public:
        enum class e_logType { LOG_ERROR, LOG_WARNING, LOG_INFO, LOG_MESSAGE };

        static FileLogger* getInstance(unsigned max_lines_count = 5, const std::string& info = "Messenger Logger", const char* fname = "logger.txt");

        std::string PrepTime();

        // Overload << operator using log type
        friend FileLogger& operator << (FileLogger& logger, const e_logType l_type) {
            ++logger.lines;
            switch (l_type) {
            case logger::FileLogger::e_logType::LOG_ERROR:
            { 
                logger.writeTime("[ERR] :\t");
                ++logger.numErrors;
            }
                break;

            case logger::FileLogger::e_logType::LOG_WARNING:
            {
                logger.writeTime("[WAR] :\t");
                ++logger.numWarnings;
            }
                break;

            case logger::FileLogger::e_logType::LOG_MESSAGE:
            {
                logger.writeTime("[MSG] :\t");
            }
                break;

            default:
            {
                logger.writeTime("[INF] :\t");
            }
                break;
            } // sw
            return logger;
        }

        void writeTime(const std::string& msg);

        friend FileLogger& operator << (FileLogger& logger, const std::string& text) {
            {
                std::lock_guard<std::mutex> guard(logger.m_writeFile);
                logger.content.back() += text + "\n";
                logger.myFile << text << std::endl;
            }
            return logger;
        }

        // Make it Non Copyable
        FileLogger(const FileLogger&) = delete;
        void operator= (const FileLogger&) = delete;

    private:
        const unsigned max_lines;
        std::atomic<unsigned> lines = 0;
        unsigned _start = 0;
        std::fstream myFile;
        time_t _now;
        std::string name;

        unsigned int numWarnings;
        unsigned int numErrors;
        std::mutex m_writeFile;
        std::deque<std::string> content;
    }; // class end

}  // namespace

#endif // FILELOGGER_HPP