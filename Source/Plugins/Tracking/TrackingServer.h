/*
    ------------------------------------------------------------------

    This file is part of the Tracking plugin for the Open Ephys GUI
    Written by:

    Alessio Buccino     alessiob@ifi.uio.no
    Mikkel Lepperod
    Svenn-Arne Dragly

    Center for Integrated Neuroplasticity CINPLA
    Department of Biosciences
    University of Oslo
    Norway

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TRACKINGSERVER_H_INCLUDED
#define TRACKINGSERVER_H_INCLUDED

#include "TrackingNode.h"

#include "oscpack/osc/OscOutboundPacketStream.h"
#include "oscpack/ip/IpEndpointName.h"
#include "oscpack/osc/OscReceivedElements.h"
#include "oscpack/osc/OscPacketListener.h"
#include "oscpack/ip/UdpSocket.h"

#include <unordered_map>
#include <memory>
class TrackingNode;

class TrackingServer: public osc::OscPacketListener,
    public Thread
{
public:
    TrackingServer (int port);
    ~TrackingServer();

    static std::shared_ptr<TrackingServer> getInstance (int port, bool justDelete = false);
    void run();
    int getIntOSC();
    float getFloatOSC();
    void addProcessor (TrackingNode* processor);
    void removeProcessor (TrackingNode* processor);

protected:
    virtual void ProcessMessage (const osc::ReceivedMessage& m, const IpEndpointName&);

private:
    TrackingServer (TrackingServer const&);
    void operator= (TrackingServer const&);

    int m_incomingPort;
    UdpListeningReceiveSocket m_listeningSocket;
    std::vector<TrackingNode*> m_processors;
    String m_address;
};


#endif  // TRACKINGSERVER_H_INCLUDED
