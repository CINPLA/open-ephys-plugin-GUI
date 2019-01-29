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
    , m_pulseDuration(2)
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

    MetaDataDescriptor md = MetaDataDescriptor(MetaDataDescriptor::UINT16, 3, "Source Channel",
            "Index at its source, Source processor ID and Sub Processor index of the channel that triggers this event", "source.channel.identifier.full");
    MetaDataValue mv(md);
    uint16 sourceInfo[3];
    sourceInfo[0] = this->getIndex();
    sourceInfo[1] = this->getNodeId();
    sourceInfo[2] = this->getIndex();
    mv.setValue(static_cast<const uint16*>(sourceInfo));
    chan->addMetaData(md, mv);
    eventChannelArray.add(chan);
}

AudioProcessorEditor* SyncNode::createEditor()
{
    editor = new SyncEditor(this, true);
    return editor;
}

void SyncNode::syncEvent()
{
    m_addEvent = true;
}


void SyncNode::setTtlDuration(int dur)
{
    m_pulseDuration = dur;
}

int SyncNode::getTtlDuration() const
{
    return m_pulseDuration;
}

void SyncNode::setOutputChan(int chan)
{
    m_chan = chan;
}

int SyncNode::getOutputChan() const
{
    return m_chan;
}

void SyncNode::process(AudioSampleBuffer& )
{    
    int64 timestamp = CoreServices::getGlobalTimestamp();
    setTimestampAndSamples(timestamp, 0);
    if (m_addEvent)
    {
        uint8 ttlData = 1 << m_chan;
        const EventChannel* chan = getEventChannel(getEventChannelIndex(0, getNodeId()));

        // Send ON event
        TTLEventPtr event = TTLEvent::createTTLEvent(chan, timestamp, &ttlData, sizeof(uint8), m_chan);
        addEvent(chan, event, 0);
        m_addEvent = false;
        CoreServices::sendStatusMessage("Sent system sync event!");

        int eventDurationSamp = static_cast<int>(ceil(m_pulseDuration / 1000.0f * getSampleRate()));
        uint8 ttlDataOff = 0;
        TTLEventPtr eventOff = TTLEvent::createTTLEvent(chan, timestamp + eventDurationSamp, &ttlDataOff, sizeof(uint8), m_chan);
        addEvent(chan, eventOff, 0);
    }
}

void SyncNode::saveCustomParametersToXml(XmlElement *parentElement)
{
    XmlElement* mainNode = parentElement->createNewChildElement("SyncNode");

    mainNode->setAttribute("output", m_chan);
    mainNode->setAttribute("duration", m_pulseDuration);

}

void SyncNode::loadCustomParametersFromXml ()
{
    if (parametersAsXml != nullptr)
    {
        forEachXmlChildElement (*parametersAsXml, mainNode)
        {
            if (mainNode->hasTagName ("SyncNode"))
            {
                m_chan = mainNode->getIntAttribute("output");
                m_pulseDuration = mainNode->getIntAttribute("duration");
                editor->updateSettings();
            }
        }
    }
}

