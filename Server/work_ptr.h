/* *********************************************************************
* class WorkPtr is for decoupling boost. 
* by wwy
* *********************************************************************/
#ifndef  MIDDLE_WRAP_H_
#define MIDDLE_WRAP_H_

namespace boost
{
    namespace asio
    {
        class io_service;
    }
}

class  WorkPtr
{
public:

    WorkPtr(boost::asio::io_service* io_svr);
    ~WorkPtr();

private:

    void* holder_;
};

#endif //MIDDLE_WRAP_H_