//
// Author       Wild Coast Solutions
//              David Hamilton
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.
//
// This file contains an implementation of a simple logging framework. 


#ifndef WILD_LOGGING_LOGGING_H
#define WILD_LOGGING_LOGGING_H

#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <initializer_list>
#include <list>
#include <chrono>
#include <iomanip>
#include <time.h>

namespace Wild
{
	namespace Logging
	{
        enum class DestinationType{
            Stdout,
            Stderr,
            File
        };

        enum class Level{
            Info,
            Debug,
            Warning,
            Error
        };

        std::ostream& operator << (std::ostream& os, const Level& l)
        {
            switch (l)
            {
            case Level::Info:       os << "Info"; break;
            case Level::Debug:      os << "Debug"; break;
            case Level::Warning:    os << "Warning"; break;
            case Level::Error:      os << "Error"; break;
            }
            return os;
        }

        class Destination
        {
        public:
            virtual void Write(const std::string &s) = 0;
        };

        class FileDestination : public Destination
        {
        public:

            FileDestination(const std::string &path)
            {
                out.open(path.c_str(), std::ios::out);
                if (!out.is_open()) throw std::runtime_error("Couldn't open file named " + path);
            }

            ~FileDestination()
            {
                out.close();
            }

            void Write(const std::string &s)
            {
                out << s << std::flush;
            }

            std::ofstream out;
        };

        class Stdout : public Destination
        {
        public:
            void Write(const std::string &s)
            {
                std::cout << s;
            }
        };

        class Stderr : public Destination
        {
        public:
            void Write(const std::string &s)
            {
                std::cerr << s;
            }
        };

        // types for adding extra info to a log message in the form of name value pairs
        typedef std::pair<std::string, std::string> I;
        typedef std::list<I> InfoBlob;

        // Generates timestamps for log messages
        std::string Timestamp()
        {
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            std::stringstream s;
 
#ifdef _WIN32
            std::tm tm;
            gmtime_s(&tm, &now);
            s << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
#else
            // only gcc > 5 supports put_time, so use alternate method
            std::tm *tm = gmtime(&now);
            char buffer[256];
            strftime(buffer, 256, "%Y-%m-%dT%H:%M:%SZ", tm);
            s << buffer;
#endif
           
            return s.str();
        }

        class Logger
        {
        public:


            // Make sure we clean up all destinations we know about, this ensure we close any open files
            ~Logger()
            {
                for (auto &destinations : m_destinations)
                {
                    for (std::shared_ptr<Destination> &i : destinations.second)
                    {
                        i.reset();
                    }
                }
            }

            // Adds a destination that prints messages to stdout
            //
            //      levels  specifies the log levels that should be passed to this destination  
            void AddStdoutDestination(std::initializer_list<Level> levels = { Level::Info, Level::Warning, Level::Error, Level::Debug })
            {
                auto destination = std::shared_ptr<Destination>(new Stdout());
                for (auto level : levels)
                {
                    m_destinations[level].push_back(destination);
                }
            }

            // Adds a destination that prints messages to stderr
            //
            //      levels  specifies the log levels that should be passed to this destination
            void AddStderrDestination(std::initializer_list<Level> levels = { Level::Info, Level::Warning, Level::Error, Level::Debug })
            {
                auto destination = std::shared_ptr<Destination>(new Stderr());
                for (auto level : levels)
                {
                    m_destinations[level].push_back(destination);
                }
            }

            // Adds a destination that prints messages to a file
            //
            //      path    name of file to write to
            //      levels  specifies the log levels that should be passed to this destination
            void AddFileDestination(const std::string &path, std::initializer_list<Level> levels = { Level::Info, Level::Warning, Level::Error, Level::Debug })
            {
                auto destination = std::shared_ptr<Destination>(new FileDestination(path));
                for (auto level : levels)
                {
                    m_destinations[level].push_back(destination);
                }
            }

            void SetDebugLevel(int debugLevel)
            {
                m_debugLevel = debugLevel;
            }

            int GetDebugLevel()
            {
                return m_debugLevel;
            }

            void Log(
                Level level,
                const std::string &doing,
                const std::string &result,
                const InfoBlob &blob)
            {
                std::stringstream s;
                s << Timestamp() << " " << level << ": "<< doing << ", " << result << ".";
                if (blob.size() > 0)
                {
                    s << " Data {";
                    auto i = blob.begin();
                    while (i != blob.end())
                    {
                        s << i->first << ": " << i->second;
                        i++;
                        if (i != blob.end()) s << ", ";
                    }
                    s << "}";
                }

                s << std::endl;

                auto destinations = m_destinations[level];
                for (auto &i : destinations)
                {
                    i->Write(s.str());
                }
            }

            void Debug(
                int debugLevel,
                const std::string &doing,
                const std::string &result,
                const InfoBlob &blob)
            {
                if (debugLevel <= m_debugLevel)
                    Log(Level::Debug, doing, result, blob);
            }

        private:

            std::map<Level, std::vector<std::shared_ptr<Destination>>> m_destinations;
            int m_debugLevel;
        };


        static std::unique_ptr<Logger> g_logger(nullptr);

        void SetupLogging(int debugLevel = 0)
        {
            g_logger.reset(new Logger());
            g_logger->AddStdoutDestination({ Level::Info, Level::Warning, Level::Debug });
            g_logger->AddStderrDestination({ Level::Error });
            g_logger->SetDebugLevel(debugLevel);
        }

        void ShutdownLogging()
        {
            g_logger.reset();
        }

        void SetDebugLevel(int debugLevel)
        {
            g_logger->SetDebugLevel(debugLevel);
        }

        int GetDebugLevel()
        {
            return g_logger->GetDebugLevel();
        }

        void AddFileDestination(const std::string &filePath)
        {
            g_logger->AddFileDestination(filePath);
        }


        void Log(
            Level level,
            const std::string &doing,
            const std::string &result,
            const std::initializer_list<I> &data = {})
        {
            g_logger->Log(level, doing, result, data);
        }

        void Log(
            Level level,
            const std::string &doing,
            const std::string &result,
            const InfoBlob &blob,
            const std::initializer_list<I> &data = {})
        {
            // Merge blob and data
            InfoBlob b = blob;
            b.insert(b.end(), data);
            g_logger->Log(level, doing, result, b);
        }



        void Info(
            const std::string &doing,
            const std::string &result,
            const std::initializer_list<I> &data = {})
        {
            Log(Level::Info, doing, result, data);
        }

        void Info(
            const std::string &doing,
            const std::string &result,
            const InfoBlob &blob,
            const std::initializer_list<I> &data = {})
        {
            Log(Level::Info, doing, result, blob, data);
        }

        void Warning(
            const std::string &doing,
            const std::string &result,
            const std::initializer_list<I> &data = {})
        {
            Log(Level::Warning, doing, result, data);
        }

        void Warning(
            const std::string &doing,
            const std::string &result,
            const InfoBlob &blob,
            const std::initializer_list<I> &data = {})
        {
            Log(Level::Warning, doing, result, blob, data);
        }

        void Error(
            const std::string &doing,
            const std::string &result,
            const std::initializer_list<I> &data = {})
        {
            Log(Level::Error, doing, result, data);
        }

        void Error(
            const std::string &doing,
            const std::string &result,
            const InfoBlob &blob,
            const std::initializer_list<I> &data = {})
        {
            Log(Level::Error, doing, result, blob, data);
        }

        void Debug(
            int debugLevel,
            const std::string &doing,
            const std::string &result,
            const std::initializer_list<I> &data = {})
        {
            g_logger->Debug(debugLevel, doing, result, data);
        }

        void Debug(
            int debugLevel,
            const std::string &doing,
            const std::string &result,
            const InfoBlob &blob,
            const std::initializer_list<I> &data = {})
        {
            // Merge blob and data
            InfoBlob b = blob;
            b.insert(b.end(), data);
            g_logger->Debug(debugLevel, doing, result, b);
        }
	}
}



#endif