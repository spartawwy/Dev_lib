#include <TLib/core/tsystem_local_logger.h>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include <TLib/core/tsystem_core_paths.h>
#include <TLib/core/tsystem_utility_functions.h>

using namespace TSystem;

const unsigned int LocalLogger::size_limit = 150*1024*1024;

static std::string GetTimeStr(const std::chrono::system_clock::time_point &time_now);

LogFile::LogFile(const std::string& name)
    : name_(name)
    , count_(0)
    , file_size_(0)
    , file_()
    , need_flush_(true)
{
}

LogFile::~LogFile()
{
}

LogFile::LogFile( LogFile&& in)
    : name_(std::move(in.name_))
    , count_(in.count_)
    , file_size_(in.file_size_)
    , file_(std::move(in.file_))
    , need_flush_(in.need_flush_)
{

}

void LogFile::Log(const std::string& msg, const std::string* p_time)
{
    file_ << *p_time << " " << msg << std::endl;
}

void LogFile::Flush()
{
    file_.flush();
}

LocalLogger::LocalLogger(const TaskPool& pool)
    : shutdown_()
    , log_strand_(pool)
    , flush_timer_(pool)
    , log_name_() 
    , log_dir_()
    , logfiles_(64)
    , log_topicdata_(nullptr) // todo:
    , msg_system_(nullptr) // todo:
{
    shutdown_ = false;
}

LocalLogger::~LocalLogger()
{
    // todo:
}

// logging
void  LocalLogger::LogLocal(const std::string& tag, const std::string& msg)
{
   std::string msg_copy = msg;
   LogLocal(tag, std::move(msg_copy));
}

void  LocalLogger::LogLocal(const std::string& tag, std::string&& msg)
{
#if 1
    auto time_now = std::chrono::system_clock::now();
    log_strand_.PostTask([tag, msg, time_now, this]()
    {
        LogFile* p_file = nullptr;
        auto iter = this->logfiles_.find(tag);
        if( iter != this->logfiles_.end() )
        {
            p_file = std::addressof(iter->second);
        }else
        {
            std::locale::global(std::locale(""));    
            LogFile  file(tag);
            utility::CreateDir(this->log_dir_ + this->log_name_);
            std::string file_full_path = this->log_dir_ + this->log_name_ + "//" + this->log_name_ + "." + tag + ".0.log";
            file.file_.open( file_full_path, std::fstream::out | std::ios::app );
            if( !file.file_.is_open() )
            {
               std::cout << "\n LocalLogger::LogLocal open " << file_full_path << " fail!" << std::endl;
               assert(false);
               return;
            }
           
            p_file = std::addressof(this->logfiles_.insert(std::make_pair(tag,  std::move(file))).first->second);
        }
        std::string time_str = GetTimeStr(time_now);
        p_file->Log(msg, &time_str);
    });
#endif
}

void  LocalLogger::LogLocal(std::string&& tag, const std::string& msg)
{
    LogLocal(tag, msg);
}

void  LocalLogger::LogLocal(std::string&& tag, std::string&& msg)
{
    LogLocal(tag, std::move(msg));
}

void  LocalLogger::LogLocal(const std::string& msg)
{
   LogLocal("app", msg);
}

void  LocalLogger::LogLocal(std::string&& msg)
{
    LogLocal("app", msg);
}

// remote logging, also log local
void  LocalLogger::LogRemote(const std::string& tag, const std::string& msg
				, LogMessage::WarnLevel level /*= LogMessage::LOG*/)
{
    // todo:
}

void  LocalLogger::LogRemote(const std::string& tag, std::string&& msg
				, LogMessage::WarnLevel level /*= LogMessage::LOG*/)
                
{
    // todo:
}
		
void  LocalLogger::LogRemote(std::string&& tag, const std::string& msg
				, LogMessage::WarnLevel level /*= LogMessage::LOG*/)
{
    // todo:
}
		
void  LocalLogger::LogRemote(std::string&& tag, std::string&& msg
				, LogMessage::WarnLevel level /*= LogMessage::LOG*/)
{
    // todo:
}

void  LocalLogger::LogRemote(const std::string& msg
				, LogMessage::WarnLevel level /*= LogMessage::LOG*/)
{
    // todo:
}

void  LocalLogger::LogRemote(std::string&& msg
				, LogMessage::WarnLevel level /*= LogMessage::LOG*/)
{
    // todo:
}

static void flash_timer_func(LocalLogger* logger, const TSystem::TError& err)
{
#if 1
    logger->StartFlush();
#endif
};
// start flushing
void LocalLogger::StartFlush()
{
    // code 
    log_strand_.PostTask([this]()
    { 
        for( auto iter = this->logfiles_.begin(); iter != this->logfiles_.end(); )
        {
            iter->second.Flush();
            auto ckval = iter->second.file_.tellg().seekpos();
            //printf("ckval:%d", ckval); 
            if( (iter->second.file_size_ = iter->second.file_.tellg().seekpos()) > LocalLogger::size_limit )
            {
                std::locale::global(std::locale(""));    
                LogFile  file(iter->second.name_);
                std::string file_full_path = utility::FormatStr("%s%s.%s.%d.log", this->log_dir_.c_str(), this->log_name_, iter->second.name_, iter->second.count_ + 1);
                file.file_.open( file_full_path, std::fstream::out | std::ios::app);
                if( !file.file_.is_open() )
                {
                   std::cout << "\n LocalLogger::StartFlush open " << file_full_path << " fail!" << std::endl;
                   assert(false);
                   continue;
                }
                file.count_ = iter->second.count_ + 1;
                this->logfiles_.erase(iter++);
                this->logfiles_.insert(std::make_pair(file.name_, std::move(file)));
            }else
                ++iter;
        }
    });
    if( !shutdown_ )
    {
        this->flush_timer_.ExpirationDuration( std::chrono::system_clock::duration(std::chrono::seconds(2)) );
        this->flush_timer_.PostTask(std::bind(flash_timer_func, this, std::placeholders::_1));
    }
}

// shutdown
void LocalLogger::Shutdown()
{
    shutdown_ = true;
}

std::string GetTimeStr(const std::chrono::system_clock::time_point& time_now)
{
    std::stringstream  ss;
        
    namespace chrono = std::chrono;
    
    // Because c-style date&time utilities don't support microsecond precison,
    // we have to handle it on our own.
    //auto time_now = chrono::system_clock::now();
    auto duration_in_mc = chrono::duration_cast<chrono::microseconds>(time_now.time_since_epoch());
    auto mc_part = duration_in_mc - chrono::duration_cast<chrono::seconds>(duration_in_mc);

    tm local_time_now;
    time_t raw_time = chrono::system_clock::to_time_t(time_now);
    _localtime64_s(&local_time_now, &raw_time);
    ss << std::put_time(&local_time_now, "%Y-%m-%d %H:%M:%S.")
            << std::setfill('0') << std::setw(3) << mc_part.count();
    return ss.str();
}