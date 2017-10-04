/*
  ==============================================================================

    ReceiveOSC.cpp
    Created: 1 Oct 2015 11:16:33am
    Author:  mikkel

  ==============================================================================
*/

#include "TrackingServer.h"
#include "TrackingNodeEditor.h"
#include "TrackingNode.h"

using std::cout;
using std::endl;

// TODO: make specific for tracking data
void TrackingServer::ProcessMessage (const osc::ReceivedMessage& receivedMessage,
                                     const IpEndpointName&)
{
    try
    {
        for (TrackingNode* processor : m_processors)
        {
            String address = processor->address();

            if ( std::strcmp ( receivedMessage.AddressPattern(), address.toStdString().c_str() ) == 0 )
            {
                osc::ReceivedMessageArgumentStream args = receivedMessage.ArgumentStream();
                std::vector<float> message;

                for (int i = 0; i < receivedMessage.ArgumentCount(); i++)
                {
                    if (receivedMessage.TypeTags()[i] == 'f')
                    {
                        float argument;
                        args >> argument;
                        message.push_back (argument);
                    }
                    else if (receivedMessage.TypeTags()[i] == 'i')
                    {
                        osc::int32 argument;
                        args >> argument;
                        message.push_back (float (argument));
                    }
                    else
                    {
                        cout << "TrackingServer: We only support floats or ints right now, not" << receivedMessage.TypeTags()[i] << endl;
                        return;
                    }
                }

                args >> osc::EndMessage;


                processor->receiveMessage (message);
                m_messagesPerSecond++;

                m_currentTime = Time::currentTimeMillis();
                m_timePassed = double (m_currentTime - m_prevTime) / 1000.0; // in seconds

                if (m_timePassed > 1.0)
                {
                    m_timePassed = 0.0;
                    m_prevTime = Time::currentTimeMillis();
                    std::cout << "OSC msg in 1sec: " << m_messagesPerSecond << std::endl;
                    m_messagesPerSecond = 0;
                }

            }
        }
    }
    catch ( osc::Exception& e )
    {
        // any parsing errors such as unexpected argument types, or
        // missing arguments get thrown as exceptions.
        DBG ("error while parsing message: " << receivedMessage.AddressPattern() << ": " << e.what() << "\n");
    }
}

int TrackingServer::getIntOSC()
{
    int test = 1;
    return test;
}

float TrackingServer::getFloatOSC()
{
    float test = 2.19;
    return test;
}

void TrackingServer::addProcessor (TrackingNode* processor)
{
    m_processors.push_back (processor);
}

void TrackingServer::removeProcessor (TrackingNode* processor)
{
    m_processors.erase (std::remove (m_processors.begin(), m_processors.end(), processor), m_processors.end());
}

TrackingServer::TrackingServer (int port)
    : Thread ("OscListener Thread")
    , m_incomingPort (port)
    , m_listeningSocket (IpEndpointName ("localhost",
                         m_incomingPort), this)
      //debug
    , m_messagesPerSecond (0)
    , m_prevTime (0)
    , m_currentTime (0)
    , m_timePassed (0.0)
{}

TrackingServer::~TrackingServer()
{
    // stop the OSC Listener thread running
    m_listeningSocket.AsynchronousBreak();

    // allow the thread 2 seconds to stop cleanly - should be plenty of time.
    stopThread (2000);
}

std::shared_ptr<TrackingServer> TrackingServer::getInstance(int port, bool justDelete)
{
    // TODO Handle case where port cannot be assigned
    static std::unordered_map<int, std::shared_ptr<TrackingServer>> instances;

    std::vector<int> toDelete;

    for (auto r : instances)
    {
        if (r.first != port && r.second->m_processors.size() < 1)
        {
            toDelete.push_back (r.first);
        }
    }

    for (auto port : toDelete)
    {
        instances.erase (port);
    }

    if (justDelete)
    {
        // the function was invoked only to delete stale instances
        return nullptr;
    }

    if (instances.count (port) < 1)
    {
        try
        {
            instances[port] = std::make_shared<TrackingServer> (port);
        }
        catch (std::runtime_error& e)
        {
            DBG ("Error unable to bind port:");
            DBG (port);
        }
    }

    if (!instances[port]->isThreadRunning())
    {
        instances[port]->startThread();
    }

    return instances[port];
}

void TrackingServer::run()
{
    // Start the oscpack OSC Listener Thread
    // NOTE: s.Run() won't return unless we force it to with
    // s.AsynchronousBreak() as is done in the destructor
    DBG ("TrackingServer: Running thread");
    m_listeningSocket.Run();
}
