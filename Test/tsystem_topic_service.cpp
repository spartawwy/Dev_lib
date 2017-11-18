#include "TLib/core/tsystem_topic_service.h"

#include "TLib/core/tsystem_core_common.h"

#include <TLib/core/tsystem_message_system.h> // nddel
#include <TLib/core/tsystem_serialization.h> // nddel
#include <TLib/core/tsystem_communication_common.h> // nddel

using namespace TSystem;
using namespace TSystem::communication;

TopicData::TopicData(const TSystem::TaskPool& pool)
    : topic_data_strand_(pool)
{

}
		
TopicData::~TopicData()
{

}

TopicData::TopicData( TopicData&& in)
    : topic_data_strand_(std::move(in.topic_data_strand_))
{

}

void TopicData::AsyncAddData( CodedDataPtr&& data )
{
    //all_data_.push_back(std::move(data));
    //auto s_z = this->all_data_.size();

    topic_data_strand_.PostTask([data, this]()
    {
        all_data_.push_back(std::move(data));
        // send 
        std::for_each( this->dests_.begin(), this->dests_.end(), [this](DestinationType& dest)
        {
            auto pos = std::get<0>(dest) == -1 ? this->all_data_.size() - 1 : std::get<0>(dest);

            auto iter_beg = this->all_data_.begin() + pos;
            auto distan = std::distance(iter_beg, this->all_data_.end());

            std::get<1>(dest)->AsyncSend(iter_beg, this->all_data_.end());

            std::get<0>(dest) = (std::get<0>(dest) == -1 ? -1 : std::get<0>(dest) + distan);
        });
    });
}

void TopicData::AsyncAddData( const CodedDataPtr& data )
{
    topic_data_strand_.PostTask([data, this]()
    {
        this->all_data_.push_back(data);
        // send 
        std::for_each( this->dests_.begin(), this->dests_.end(), [this](DestinationType& dest)
        {
            auto pos = std::get<0>(dest) == -1 ? this->all_data_.size() - 1 : std::get<0>(dest);

            auto iter_beg = this->all_data_.begin() + pos;
            auto distan = std::distance(iter_beg, this->all_data_.end());

            std::get<1>(dest)->AsyncSend(iter_beg, this->all_data_.end());

            std::get<0>(dest) = (std::get<0>(dest) == -1 ? -1 : std::get<0>(dest) + distan);
        });
    });
}

// add destination, will start sending procedure if pos = first / last
void TopicData::AsyncAddDestination( std::shared_ptr<Connection>&& pconn, DataStartPos pos )
{
    const std::shared_ptr<Connection> cst_pconn = std::move(pconn);
    AsyncAddDestination(cst_pconn, pos);
}

void TopicData::AsyncAddDestination( const std::shared_ptr<Connection>& pconn, DataStartPos pos)
{
    topic_data_strand_.PostTask([pos, pconn, this]()
    {
        pconn->holder_remover_=  std::bind(&TopicData::AsyncRemoveDestination, this, std::placeholders::_1);
        switch(pos)
        {
        case DataStartPos::FIRST:
            {
                auto dest = (DestinationType)std::make_tuple(0, std::move(pconn));
                if( this->all_data_.size() > 0 )
                { 
#if 1 // nddel tmp codes 
                    
                    std::for_each(std::begin(this->all_data_), std::end(this->all_data_), [this](CodedDataPtr& data_ptr)
                    {
                        //TSystem::Message msg("", 10); 
                        TSystem::Message msg(data_ptr->data(), data_ptr->size());
                        printf("TopicData::AsyncAddDestination msg.serial_data().ClassID()-1 : %d \n", msg.serial_data().ClassID()-1);
                    });
                    

#endif 
                    std::get<1>(dest)->AsyncSend(this->all_data_.begin(), this->all_data_.end());
                    std::get<0>(dest) = this->all_data_.size();
                }
                this->dests_.push_back( std::move(dest) );
            }
            break;
        case DataStartPos::LAST:
            {
                DestinationType dest;
                if( this->all_data_.size() > 0 )
                {
                    dest = (DestinationType)std::make_tuple(all_data_.size()-1, std::move(pconn));
                    std::get<1>(dest)->AsyncSend(this->all_data_.begin(), this->all_data_.end());
                    std::get<0>(dest) = this->all_data_.size();

                }else
                    dest = (DestinationType)std::make_tuple(0, std::move(pconn));
                this->dests_.push_back( std::move(dest) );
            }
            break;
        case DataStartPos::LATEST:
            {
                this->dests_.push_back( std::move((DestinationType)std::make_tuple(-1, std::move(pconn))) );
            }
            break;
        default: assert(0);break;
        }
        // send data for this connection

        
    });
}

// remove destination
void TopicData::AsyncRemoveDestination( int conid)
{
    topic_data_strand_.PostTask([conid, this]()
    {
        auto iter = std::find_if(this->dests_.begin(), this->dests_.end(), [conid](DestinationType& dst)
        {
            return std::get<1>(dst)->connid() == conid;
        });
        if( iter != this->dests_.end() )
            this->dests_.erase(iter); 
    });
}

// update new data (old last data is replaced) and send to all
void TopicData::AsyncUpdateData( CodedDataPtr&& data )
{ 
    //auto len = data->size(); 
    
    topic_data_strand_.PostTask([data, this]()
    {
        std::for_each( dests_.begin(), dests_.end(), [data, this](DestinationType &entry)
        {
            //std::get<0>(entry);
            std::get<1>(entry)->AsyncSend(data);
        });
    });
}

void TopicData::AsyncUpdateData( const CodedDataPtr& data)
{
    topic_data_strand_.PostTask([data, this]()
    {
        std::for_each( dests_.begin(), dests_.end(), [data, this](DestinationType &entry)
        {
            //std::get<0>(entry);
            std::get<1>(entry)->AsyncSend(data);
        });
    });
}

// send latest to one dest
void TopicData::AsyncSendOne( std::shared_ptr<Connection>&& pconn, DataStartPos pos)
{
    const std::shared_ptr<Connection> connect(std::move(pconn));
    AsyncSendOne(connect, pos); 
}

void TopicData::AsyncSendOne( const std::shared_ptr<Connection>& pconn, DataStartPos pos)
{
    if( all_data_.size()  < 1 )
        return;
    topic_data_strand_.PostTask([pconn, pos, this]()
    {  
        switch(pos)
        {
            case DataStartPos::FIRST:
                pconn->AsyncSend( all_data_.at(0) );
                break;
            case DataStartPos::LAST:
                pconn->AsyncSend( all_data_.at(all_data_.size()-1) );
                break;
            case DataStartPos::LATEST:
                pconn->AsyncSend( all_data_.at(all_data_.size()-1) );
                break;
        } 
    });
}

// add data into queue
void TopicData::AddData(  CodedDataPtr& data )
{
    // todo:
}

// send queue
void TopicData::SendData()
{
    // todo:
}

// add destination
void TopicData::AddDestination( std::shared_ptr<Connection>& pconn, DataStartPos pos )
{
    // todo:
}

// remove destination
void TopicData::RemoveDestination(int conid)
{
    // todo:
}

// update data and send to all
void TopicData::UpdateData( CodedDataPtr& data)
{
    // todo:
}

// send data once
void TopicData::SendOne( std::shared_ptr<Connection>& pconn, DataStartPos pos )
{
    // todo:
}


//-------------------
// utility functions
//-------------------

// send data queue to one connection, return if connection's sending progress is done
bool TopicData::SendDataToOne(DestinationType& dest)
{
    // ndchk: 
    std::get<1>(dest)->AsyncSend(all_data_.begin(), all_data_.end()); 
     
    return true;
}
		
// wrapper of SendDataToOne, but check if connection is connected before send
void TopicData::CheckAndSendDataToOne(DestinationType& dest)
{
    // ndchk:
    if( std::get<1>(dest)->status() == communication::Connection::Status::connected )
        SendDataToOne(dest);
}

TopicData::DataStartPos communication::IntToDataStartPos(int c)
{
    // ndchk:
    switch(c)
    {
     case 'h': return TopicData::DataStartPos::FIRST; 
     case 't': return TopicData::DataStartPos::LAST;
     case 'l': return TopicData::DataStartPos::LATEST;
     default:
         ThrowTException( CoreErrorCategory::ErrorCode::BAD_CONTENT
				, "IntToDataStartPos"
				, "c is out of range");
         return TopicData::DataStartPos::LATEST;
    }
}




/////////////////////////////////////////////////////////////////////////////
// class of Topic
/////////////////////////////////////////////////////////////////////////////

Topic::Topic()
{
    // todo:
}

Topic::~Topic()
{
    // todo:
}

TopicData& Topic::AddTopicData( const std::string& key, const TaskPool& pool)
{
    auto iter = topic_data_.find(key);
    if( iter != topic_data_.end() )
        return *iter->second;
    else
        return *topic_data_.insert( std::make_pair(key, std::unique_ptr<TopicData>(new TopicData(pool))) ).first->second;
}

// find a topic data
TopicData* Topic::Find(const std::string& key)
{
    auto iter = topic_data_.find(key);
    if( iter != topic_data_.end() )
        return iter->second.get();
    else
        return nullptr;
}

// add destination for all TopicData
void Topic::AddDestinationForAll( const std::shared_ptr<Connection>& pconn, TopicData::DataStartPos pos )
{
    std::for_each( std::begin(topic_data_), std::end(topic_data_), [&pconn, pos, this](std::unordered_map<std::string, std::unique_ptr<TopicData>>::reference entry)
    {
        entry.second->AsyncAddDestination(pconn, pos);
    });
}

// remove destination from all TopicData
void Topic::RemoveDestinationFromAll( int connid )
{
    std::for_each( std::begin(topic_data_), std::end(topic_data_), [connid, this](std::unordered_map<std::string, std::unique_ptr<TopicData>>::reference entry)
    {
        entry.second->AsyncRemoveDestination(connid);
    });
}

// send one for all topic data
void Topic::SendOneForAll(  const std::shared_ptr<Connection>& pconn, TopicData::DataStartPos pos )
{
    std::for_each( std::begin(topic_data_), std::end(topic_data_), [&pconn, pos, this](std::unordered_map<std::string, std::unique_ptr<TopicData>>::reference entry)
    {
        entry.second->AsyncSendOne(pconn, pos);
    });
}

TopicServer::TopicServer()
    : topics_(256)
{
    // todo:
}

TopicServer::~TopicServer()
{
    // todo:
}

// initiate
Topic& TopicServer::AddTopic(unsigned short topic_id, const TaskPool& pool)
{ 
    auto iter = topics_.find(topic_id);
    if( iter != topics_.end() )
        return *iter->second;
    else
    {
        return *(topics_.insert( std::make_pair(topic_id, std::unique_ptr<Topic>(new Topic())) ).first->second);
    }
}

TopicData& TopicServer::AddTopicData( unsigned short topic_id, const std::string& key, const TaskPool& pool)
{ 
    return AddTopic(topic_id, pool).AddTopicData(key, pool);
}

// find topic data
TopicData* TopicServer::Find(unsigned short topic_id, const std::string& key)
{
    auto p_topic = Find(topic_id); 
    if( p_topic )
        return p_topic->Find(key);
    else
        return nullptr;
}

// find topic
Topic*    TopicServer::Find(unsigned short topic_id)
{
    auto iter = topics_.find(topic_id);
    if( iter != topics_.end() )
        return iter->second.get();
    else
        return nullptr;
}