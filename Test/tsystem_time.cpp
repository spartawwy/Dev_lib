
#include <TLib/core/tsystem_time.h>

#include <iomanip>
#include <sstream>

#include <TLib/core/tsystem_error.h>
#include <TLib/core/tsystem_core_error.h>
#include <TLib/core/tsystem_utility_functions.h>

namespace TSystem
{
    class FakeClock
    {
    public:
        FakeClock(){}
        // todo: other functions
    };
}

#include <TLib/core/tsystem_time.h>

using namespace TSystem;
using namespace std::chrono;

// todo: need check
SystemClock g_system_clock;

// constructor
SystemClock::SystemClock(): fake_imp_(nullptr)
{

}
SystemClock::~SystemClock()
{

}

// set to fake mode
// t0: the start time point of fake time
// speed: speed of time elapse
void SystemClock::FakeMode(std::chrono::system_clock::time_point t0, unsigned short speed)
{

}

// reset start time point of fake mode
bool SystemClock::ResetFake()
{
    // todo:
    return true;
}

// set to real mode
void SystemClock::RealMode()
{
    // todo:
}

// is real mode
bool SystemClock::IsReal() const
{
    // todo:
    return true;
}

// get current time point
std::chrono::system_clock::time_point SystemClock::Now() const
{
    return std::chrono::system_clock::now();
}

SystemClock& TSystem::GetClock()
{
    return g_system_clock;
}

TimePoint::TimePoint( const std::chrono::system_clock::time_point& tpoint)
{
    frac_sec_ = tpoint.time_since_epoch(); // todo: what's frac_sec ?

    time_t_ = std::chrono::system_clock::to_time_t(tpoint);
    tm_ = *localtime(&time_t_);
}

std::string TimePoint::Str() const
{
    std::stringstream ss;
    std::tm tm = *std::localtime(&time_t_);
    auto time = std::put_time(const_cast<std::tm*>(&tm_), "%b %d %Y %H:%M:%S");
    ss << time;
    return ss.str();
}

// make time_point from input, use this to test if is a valid time point
bool TSystem::MakeTimePoint( std::chrono::system_clock::time_point* tp, TError& ec, int year, 
                            int month, int day, 
                            int hour, int min, int sec, int mil)
{
    try
    {
        *tp = MakeTimePoint(year, month, day, hour, min, sec, mil);

    }catch(TException& e)
    {
        ec = *const_cast<TError*>(&e.error());
        return false;
    }
    return true;
}

// throw TException version
std::chrono::system_clock::time_point TSystem::MakeTimePoint( int year, 
                                                             int month, int day, 
                                                             int hour, int min, int sec, int mil)
{
    // todo: need check millisecondes
    struct tm t_m;
    t_m.tm_year = year - 1900;
    t_m.tm_mon = month - 1;
    t_m.tm_mday = day;
    t_m.tm_hour = hour;
    t_m.tm_min = min;
    t_m.tm_sec = sec;

    system_clock::time_point  tp;
    try
    {
        system_clock::duration dura(std::chrono::milliseconds(std::mktime(&t_m) * 1000 + mil));
        tp += dura;

    }catch(...)
    {
        ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
            , "MakeTimePoint"
            , utility::FormatStr("month:%d day:%d hour:%d min:%d sec:%d mil:%d", month, day, hour, min, sec, mil));
    }
    /*std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>  tp;
    tp += dura;*/
    return tp;
}

// make time_point from time_t and fraction of second
std::chrono::system_clock::time_point TSystem::MakeTimePoint( time_t a, double b)
{
    // todo: check

    // compile erro, means can't return tp ?
    //std::chrono::duration<double> dida(b);
    //auto val = std::chrono::system_clock::from_time_t(a) + dida;
    //std::chrono::time_point<system_clock, double> tp(std::chrono::duration<double>(val));
    //return tp;

    return std::chrono::system_clock::from_time_t(a);
}


TimeCount::TimeCount()
{
}

void TimeCount::Start()
{
    start_ = std::chrono::system_clock::now();
}

double TimeCount::Count() const
{
    return static_cast<double>(start_.time_since_epoch().count());
}

DateComponent TSystem::FromLongdate(int longdate)
{
    return DateComponent(longdate/10000, (longdate%10000)/100, longdate%100);
}

// componenet to longdate
int TSystem::ToLongdate(int yyyy, int mm, int dd)
{
    return  yyyy*10000 + mm*100 + dd;
}

// today 
int TSystem::Today()
{
#ifdef WIN32
    auto tm_t = system_clock::to_time_t(std::chrono::system_clock::now());
    auto st_tm = localtime(&tm_t);
    return st_tm->tm_year*10000 + st_tm->tm_mon*100 + st_tm->tm_mday;
#else
    tm local_time_now;
    time_t raw_time = system_clock::to_time_t(std::chrono::system_clock::now());
    _localtime64_s(&local_time_now, &raw_time);
    return (local_time_now.tm_year + 1900) * 10000 + (local_time_now.tm_mon + 1) * 100 + local_time_now.tm_mday;
#endif
}

void TSystem::FillTime(const TimePoint& t, Time& msg)
{	
    msg.set_time_value(t.TimeValue());
    msg.set_frac_sec(t.frac_sec());
}

// output network time message
std::string TSystem::ToString(const Time& t)
{
    TimePoint tp(std::chrono::system_clock::from_time_t(t.time_value()));
    return tp.Str();
}

