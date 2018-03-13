#ifndef PERSIST_SERVICE_H_
#define PERSIST_SERVICE_H_

#include <future>
#include <vector>
#include <queue>
#include <mutex>    
#include <condition_variable> 

#ifdef USE_BOOST_THREAD
#include <boost/thread.hpp>
#else
#include <thread>
#endif
#include <boost/asio/strand.hpp>
#include <boost/asio/io_service.hpp>

#include <TLib/core/tsystem_core_error.h>
#include <TLib/core/tsystem_task_service.h>

namespace boost
{
    namespace asio
    {
        class io_service;
    }
}

namespace  TSystem
{
    class TaskPool;

    ///typedef std::function<void()>  QueTask;
    typedef std::function<void(const TSystem::TError&)>  TimedTaskInner;
    //class QueuedTask;

    class PersistService
    {
        friend class TaskPool;
    public:
        explicit PersistService();
        ~PersistService();

        void AddWorker();
        void AddNormalWorker();
        void Join();
        void Stop();

        boost::asio::io_service& service();
#if 0 
        //template<typename F, typename ... Args>    
       // add new work item to the pool    
	    template<typename F, typename Args>    
	    auto enqueue(F&& f, Args&& args) -> std::future< typename std::result_of<F(Args)>::type >    
	    {    
	       //using return_type = typename std::result_of<F(Args)>::type;    
	       typedef typename std::result_of<F(Args)>::type  return_type;
	       auto task = std::make_shared< std::packaged_task<return_type()> >(    
			       std::bind(std::forward<F>(f), std::forward<Args>(args))    
		       );    
           
	       std::future<return_type> res = task->get_future();    

	       {    
		       std::unique_lock<std::mutex> lock(queue_mutex);    
		       // don't allow enqueueing after stopping the pool    
		       if(stop_)    
			       throw std::runtime_error("enqueue on stopped ThreadPool");    
		       tasks.emplace([task](){ (*task)(); });    
	       }    
	       condition.notify_one();    
	       return res;    
	    }  
#else
        template<typename F>    
	    auto enqueue(F&& f) -> std::future< typename std::result_of<F()>::type >    
	    {    
	       //using return_type = typename std::result_of<F(Args)>::type;    
	       typedef typename std::result_of<F()>::type  return_type;
	       auto task = std::make_shared< std::packaged_task<return_type()> >(    
			       std::bind(std::forward<F>(f))    
		       );    
           
	       std::future<return_type> res = task->get_future();    

	       {    
		       std::unique_lock<std::mutex> lock(queue_mutex);    
		       // don't allow enqueueing after stopping the pool    
		       if(stop_)    
			       throw std::runtime_error("enqueue on stopped ThreadPool");    
		       tasks.emplace([task](){ (*task)(); });    
	       }    
	       condition.notify_one();    
	       return res;    
	    }  
#endif
        size_t WorkerSize() const { return workers.size(); }

        //20170517
        void DispatchTask(QueuedTask&& task);

    private:    

       size_t normal_threads_num_;
       // need to keep track of threads so we can join them    
#ifdef USE_BOOST_THREAD
       std::vector< boost::thread > workers;   
#else
       std::vector< std::thread > workers;   
#endif 
       // the task queue    
       std::queue< std::function<void()> > tasks;    
       
       // synchronization    
       std::mutex queue_mutex;    
       std::condition_variable condition;    
       bool stop_;    

       boost::asio::io_service::work * work_; //ndchk
       //20170517
       boost::asio::strand  strand_;
    };


    class ServiceStrand
    {
    public:

        explicit ServiceStrand(const TaskPool& pool);
        /*inline*/ void Post(QueuedTask&& task);
        /*inline*/ void Post(const QueuedTask& task) const;
        /*inline*/ void Dispatch(QueuedTask&& task);
        /*inline*/ void Dispatch(const QueuedTask& task) const;

    private:

        std::unique_ptr<boost::asio::strand> p_strand_;
    };

    class ServiceTimer
    {
    public:

        explicit ServiceTimer(const TaskPool& pool);
        // cancel all async waiting
		void Cancel() const;

		// set expire time, will cancel any async waiting
		void ExpirationTimePoint(const std::chrono::system_clock::time_point& t);
		void ExpirationDuration(const std::chrono::system_clock::duration& dur);

		// async wait
		void PostTask(TimedTaskInner&& timed_task) const;
		void PostTask(const TimedTaskInner& timed_task) const;

    private:
        enum class ExpiraType: char
        {
            DURATION,
            TIMEPOINT
        };
        
        const TaskPool&  pool_;
        //std::queue<TimedTaskInner>  timed_tasks_;
        std::chrono::system_clock::duration  dura_;
        std::chrono::system_clock::time_point  tp_;

        mutable ExpiraType  expira_type_;
        mutable bool  cancel_flag_;
    };

    class ServiceSignalSet
    {
    public:
        ServiceSignalSet(const TaskPool& pool) : pool_(pool) { }

    private:

        const TaskPool&  pool_;
    };
}
#endif // TSYSTEM_SERVICES_H_
