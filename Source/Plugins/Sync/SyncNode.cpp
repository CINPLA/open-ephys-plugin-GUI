/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2014 Open Ephys

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

#include "SyncNode.h"
#include "SyncEditor.h"


SyncNode::SyncNode()
    : GenericProcessor("Sync Port")
    , m_addEvent(false)
    , m_chan(0)
{
    setProcessorType (PROCESSOR_TYPE_SOURCE);
}

SyncNode::~SyncNode()
{
}

//Since the data needs a meximum buffer size but the actual number of read bytes might be less, let's
//add that info as a metadata field.
void SyncNode::createEventChannels()
{
    EventChannel* chan = new EventChannel(EventChannel::TTL, 8, 1, CoreServices::getGlobalSampleRate(), this);
    chan->setName("Sync TTL event");
    chan->setDescription("Sync signal from Sync plugin");
    chan->setIdentifier("internal.sync.ttl");
    eventChannelArray.add(chan);
}

AudioProcessorEditor* SyncNode::createEditor()
{
    editor = new SyncEditor(this, true);
    return editor;
}

void SyncNode::syncEvent(int chan)
{
    m_chan = chan;
    m_addEvent = true;
}

void SyncNode::process(AudioSampleBuffer& )
{    
    int64 timestamp = CoreServices::getGlobalTimestamp();
    setTimestampAndSamples(timestamp, 0);
    if (m_addEvent)
    {
        uint8 ttlData = 1 << m_chan;
        const EventChannel* chan = getEventChannel(getEventChannelIndex(0, getNodeId()));
        std::cout << chan->getDataSize() << std::endl;

        TTLEventPtr event = TTLEvent::createTTLEvent(chan, timestamp, &ttlData, sizeof(uint8), m_chan);
        addEvent(chan, event, 0);
        m_addEvent = false;
        CoreServices::sendStatusMessage("Sent system sync event!");
    }
}
