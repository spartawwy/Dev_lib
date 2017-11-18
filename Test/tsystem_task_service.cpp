#include "persist_service.h"
#include <TLib/core/tsystem_task_service.h>

#include <boost/asio/strand.hpp>

#include "service_signal_set.h"



using namespace TSystem;

TaskPool::TaskPool() : pool_(new PersistService)
{
     
}


TaskPool::~TaskPool()
{

}

TaskPool::TaskPool( TaskPool&& in ) : pool_(std::move(in.pool_))
    , workers_(std::move(in.workers_))
{

}

PersistService& TaskPool::Pool() const
{
    return *pool_;
}

void TaskPool::AddWorker()
{
    pool_->AddWorker();
}

void TaskPool::AddThisThread()
{
    // TODO:
}

size_t TaskPool::RunOneInThisThread()
{
    // TODO:
   return 0;
}

void TaskPool::PostTask(QueuedTask&& task) const
{
    //todo: ndchk
    pool_->enqueue(task);
}

void TaskPool::DispatchTask(QueuedTask&& task)
{
    pool_->DispatchTask(std::move(task));
}

void TaskPool::DispatchTask(const QueuedTask& task) const
{
    assert(0);// todo
}
size_t TaskPool::WorkerSize() const
{
    return pool_->WorkerSize();
}

void TaskPool::Shutdown()
{
    pool_->Stop();
}

void TaskPool::FireShutdown()
{
    // TODO:
    pool_->Stop();
}

bool TaskPool::HasShutdown() const
{
    // TDOO:
   return pool_->stop_ == true;
}

void TaskPool::Join()
{
    pool_->Join();
}

// check if worker thread has exited because of too many execution 
		// or terminated because of uncaught exception 
		// will fill the worker if the first case 
		// and throw the uncaught exception in the second case
void TaskPool::CheckWorkers()
{
    // todo: 
}

////////////////////////////////////////////////////////////////////////
// TaskStrand
//////////////////////////////////////////////////////////////////////// 
TaskStrand::TaskStrand( const TaskPool& pool )
    : task_pool_(pool)
    , strand_(new ServiceStrand(pool))
{

}

TaskStrand::~TaskStrand()
{

}

TaskStrand::TaskStrand( TaskStrand&& in )
    : task_pool_(std::move(in.task_pool_))
    , strand_(std::move(in.strand_))
{

}

const TaskPool& TaskStrand::GetTaskPool() const
{
    return task_pool_;
}

void TaskStrand::PostTask(QueuedTask&& task) const
{
    strand_->Post(std::move(task));
}

void TaskStrand::PostTask(const QueuedTask& task) const
{
    strand_->Post(task);
}

void TaskStrand::DispatchTask(QueuedTask&& task) const
{
    strand_->Dispatch(std::move(task));
}

void TaskStrand::DispatchTask(const QueuedTask& task) const
{
    strand_->Dispatch(task);
}

////////////////////////////////////////////////////////////////////////
// TaskTimer
////////////////////////////////////////////////////////////////////////
TaskTimer::TaskTimer(const TaskPool& pool)
    : task_pool_(pool)
    , timer_(new ServiceTimer(pool))
{
}

TaskTimer::~TaskTimer()
{
}

TaskTimer::TaskTimer( TaskTimer&& in )
    : task_pool_(in.task_pool_)
    , timer_(std::move(in.timer_))
{
}

// the task pool
const TaskPool& TaskTimer::GetTaskPool() const
{
    return task_pool_;
}

// cancel all async waiting
void TaskTimer::Cancel() const
{ 
    timer_->Cancel();
}

// set expire time, will cancel any async waiting
void TaskTimer::ExpirationTimePoint(const std::chrono::system_clock::time_point& t) const
{
    timer_->ExpirationTimePoint(t);
}

void TaskTimer::ExpirationDuration(const std::chrono::system_clock::duration& dur) const
{
    timer_->ExpirationDuration(dur);
}

// async wait
void TaskTimer::PostTask(TimedTask&& timed_task) const
{
    timer_->PostTask(std::move(timed_task));
}

void TaskTimer::PostTask(const TimedTask& timed_task) const
{
    timer_->PostTask(timed_task);
}

////////////////////////////////////////////////
//  TaskSignalSet
///////////////////////////////////////////////
TaskSignalSet::TaskSignalSet(const TaskPool& pool)
    : task_pool_(pool)
    , signal_set_(new ServiceSignalSet(pool))
{

}

TaskSignalSet::~TaskSignalSet()
{
}

// moving constructor
TaskSignalSet::TaskSignalSet(TaskSignalSet&& in)
    : task_pool_(in.task_pool_)
    , signal_set_(std::move(in.signal_set_))
{
    // todo: 
}

// the task pool
const TaskPool& TaskSignalSet::GetTaskPool() const
{
     return task_pool_;
}

// cancel all async waiting
void TaskSignalSet::Cancel() const
{
    // todo:
}

// add signal, may throw TException if add fail
void TaskSignalSet::Add(int signal_number) const
{
    // todo: 
}

// async wait
void TaskSignalSet::PostTask( SignaledTask&& signaled_task ) const
{
    // todo:
}

void TaskSignalSet::PostTask( const SignaledTask& signaled_task) const
{
   // todo:
}