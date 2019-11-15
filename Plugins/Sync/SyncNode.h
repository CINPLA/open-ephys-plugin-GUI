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

#ifndef SYNCNODE_H
#define SYNCNODE_H

#include <ProcessorHeaders.h>

#include <stdio.h>

/**
    This source processor allows you to generate TTL events and save their timestamp.

    @see SerialInputEditor
*/
class SyncNode : public GenericProcessor
{
public:

    SyncNode();
    ~SyncNode();

    AudioProcessorEditor* createEditor();

    void process(AudioSampleBuffer& buffer) override;
    void syncEvent();
    void setTtlDuration(int dur);
    int getTtlDuration() const;
    void setOutputChan(int chan);
    int getOutputChan() const;

    void saveCustomParametersToXml(XmlElement *parentElement);
    void loadCustomParametersFromXml();

protected:
    void createEventChannels() override;

private:

    bool m_addEvent;
    int m_chan;
    int m_pulseDuration;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SyncNode);

};


#endif  // SYNCNODE_H
