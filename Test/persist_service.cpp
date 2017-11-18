#include "persist_service.h"

#include <thread>
#include <algorithm>

using namespace TSystem;

static boost::asio::io_service  g_io_service;
 
//static const unsigned int cst_service_thread_num = 2;

PersistService::PersistService()
    : normal_threads_num_(5)
    , stop_(false)
    , work_(new boost::asio::io_service::work(g_io_service))
    , strand_(g_io_service)
{
#if 1
    for(size_t i = 0; i < 2; ++i) 
        AddWorker();
#endif
#if 1
    for(size_t i = 0; i < normal_threads_num_; ++i)    
        AddNormalWorker();  
#endif 
}

void PersistService::AddWorker()
{
    workers.emplace_back(
        [this]
    {
        while( !this->stop_ ) 
        { 
            //std::this_thread::sleep_for(std::chrono::system_clock::duration(std::chrono::milliseconds(100)));
            g_io_service.run(); 
        }
    });
}

void PersistService::AddNormalWorker()
{
    workers.emplace_back(    
        [this]    
    {    
        for(;;)    
        {    
            std::function<void()> task;   

            {    
                std::unique_lock<std::mutex> lock(this->queue_mutex);   

                this->condition.wait(lock,    
                    [this]{ return this->stop_ || !this->tasks.empty(); });    

                if( this->stop_ /*&& this->tasks.empty()*/)    
                    return;    

                task = std::move(this->tasks.front());    
                this->tasks.pop();    
            }    
            task();    
        }    
    }    
    );  
    
}

void PersistService::Join()
{
    std::for_each(std::begin(workers), std::end(workers), [](std::thread &entry)
    {
        entry.join();
    });
}

void PersistService::Stop()
{
    stop_  = true;
    condition.notify_all();
    if( work_ )
    {
        delete work_; 
        work_ = nullptr;
    }
}

PersistService::~PersistService()
{
    Stop();
}

boost::asio::io_service& PersistService::service()
{
    return g_io_service;
}

void PersistService::DispatchTask(QueuedTask&& task)
{
    strand_.dispatch( strand_.wrap( task) );

    //boost::mutex::scoped_lock lock(io_mutex);
}

////////////////////////////////////////////////////////
//  ServiceStrand
////////////////////////////////////////////////////////

ServiceStrand::ServiceStrand(const TaskPool& pool)
{
    p_strand_ = std::unique_ptr<boost::asio::strand>(new boost::asio::strand(g_io_service));
}

void ServiceStrand::Post(QueuedTask&& task)
{
    p_strand_->post(std::move(task));
}

void ServiceStrand::Post(const QueuedTask& task) const
{
    p_strand_->dispatch(task);
}

void ServiceStrand::Dispatch(QueuedTask&& task)
{
    p_strand_->dispatch(std::move(task));
}

void ServiceStrand::Dispatch(const QueuedTask& task) const
{
    p_strand_->dispatch(task);
}

////////////////////////////////////////////////////////
//  ServiceTimer
////////////////////////////////////////////////////////

ServiceTimer::ServiceTimer(const TaskPool& pool)
    : pool_(pool)
    //, timed_tasks_()
    , dura_()
    , tp_()
    , expira_type_(ExpiraType::DURATION)
    , cancel_flag_(false)
{

}

// cancel all async waiting
void ServiceTimer::Cancel() const
{
    cancel_flag_ = true;
}

// set expire time, will cancel any async waiting
void ServiceTimer::ExpirationTimePoint(const std::chrono::system_clock::time_point& t) 
{
    expira_type_ = ExpiraType::TIMEPOINT;
    tp_ = t;
}

void ServiceTimer::ExpirationDuration(const std::chrono::system_clock::duration& dur) 
{
    expira_type_ = ExpiraType::DURATION;
    //dura_ = *const_cast<std::chrono::system_clock::duration*>(&dur);
    dura_ = dur;
}

// async wait
void ServiceTimer::PostTask(TimedTask&& timed_task) const
{
    cancel_flag_ = false;

    pool_.PostTask(
        [timed_task, this]()
    { 
        if( expira_type_ == ExpiraType::TIMEPOINT )
            std::this_thread::sleep_until(this->tp_);
        else
            std::this_thread::sleep_for(this->dura_);
        if( !this->cancel_flag_ )
            timed_task(MakeTError(CoreErrorCategory::ErrorCode::OK, "", ""));
    }); 

}

void ServiceTimer::PostTask(const TimedTask& timed_task) const
{
    // todo:
    cancel_flag_ = false;

    pool_.PostTask(
        [timed_task, this]()
    { 
        if( expira_type_ == ExpiraType::TIMEPOINT )
            std::this_thread::sleep_until(this->tp_);
        else
            std::this_thread::sleep_for(this->dura_);
        if( !this->cancel_flag_ )
            timed_task(MakeTError(CoreErrorCategory::ErrorCode::OK, "", ""));
    }); 
}
