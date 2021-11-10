#ifndef FILELOGGER_HPP
#define FILELOGGER_HPP

#include <fstream>
#include <string>
#include <chrono>
#include <mutex>
#include <atomic>

// Use the namespace you want
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

        static FileLogger* getInstance(const std::string& info, const char* fname = "logger.txt", unsigned max_lines_count = 5);

        std::string PrepTime();

        // Overload << operator using log type
        friend FileLogger& operator << (FileLogger& logger, const e_logType l_type) {
            ++logger.lines;
            switch (l_type) {
            case logger::FileLogger::e_logType::LOG_ERROR:
            {
                std::lock_guard<std::mutex> guard(logger.m_writeFile);
                logger.myFile << logger.PrepTime() << "[ERROR] :\t";
                ++logger.numErrors;
            }
                break;

            case logger::FileLogger::e_logType::LOG_WARNING:
            {
                std::lock_guard<std::mutex> guard(logger.m_writeFile);
                logger.myFile << logger.PrepTime() << "[WARNING] :\t";
                ++logger.numWarnings;
            }
                break;

            case logger::FileLogger::e_logType::LOG_MESSAGE:
            {
                std::lock_guard<std::mutex> guard(logger.m_writeFile);
                logger.myFile << logger.PrepTime() << "[MESSAGE] :\t";
            }
                break;

            default:
            {
                std::lock_guard<std::mutex> guard(logger.m_writeFile);
                logger.myFile << logger.PrepTime() << "[INFO] :\t";
            }
                break;
            } // sw
            if (logger.lines > logger.max_lines) {
                //logger.myFile.seekp(logger._start, std::ios_base::beg);
                //std::string toDelete;
                //std::getline(logger.myFile, toDelete);
                /*logger.myFile << "kek";
                logger.myFile.seekp(0, std::ios_base::end);*/
                --logger.lines;
            }
            return logger;
        }

        friend FileLogger& operator << (FileLogger& logger, const std::string& text) {
            logger.myFile << text << std::endl;
            return logger;
        }

        // Make it Non Copyable (or you can inherit from sf::NonCopyable if you want)
        FileLogger(const FileLogger&) = delete;
        void operator= (const FileLogger&) = delete;

    private:
        const unsigned max_lines;
        std::atomic<unsigned> lines = 0;
        unsigned _start = 0;
        std::fstream myFile;
        time_t _now;

        unsigned int numWarnings;
        unsigned int numErrors;
        std::mutex m_writeFile;

    }; // class end

}  // namespace

#endif // FILELOGGER_HPP