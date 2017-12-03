/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2016 Open Ephys

    Modified by:

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

#ifndef __PULSEPALOUTPUT_H_A8BF66D6__
#define __PULSEPALOUTPUT_H_A8BF66D6__

#include <ProcessorHeaders.h>
#include "PulsePalOutputEditor.h"
#include "serial/PulsePal.h"

#define DEF_PHASE_DURATION 1
#define DEF_INTER_PHASE 1
#define DEF_INTER_PULSE 5
#define DEF_REPETITIONS 1
#define DEF_TRAINDURATION 100
#define DEF_VOLTAGE 5
#define DEF_BURSTDURATION 30
#define DEF_INTER_BURST 20

#define MAX_VOLTAGE 10
#define MAX_INTERVAL 3600000
#define PULSEPALCHANNELS 4

struct EventSources
{
    unsigned int eventIndex;
    unsigned int sourceId;
    unsigned int channel;
};

/**
    Allows the signal chain to send outputs to the Pulse Pal
    from Lucid Biosystems (www.lucidbiosystems.com)

    @see GenericProcessor, PulsePalOutputEditor, PulsePal
*/
class PulsePalOutput : public GenericProcessor
{
public:
    PulsePalOutput();
    ~PulsePalOutput();

    AudioProcessorEditor* createEditor() override;
    void process (AudioSampleBuffer& buffer) override;
    void setParameter (int parameterIndex, float newValue) override;
    void handleEvent (const EventChannel* eventInfo, const MidiMessage& event, int sampleNum) override;

    // Pulse Pal
    bool updatePulsePal(int chan);

    bool getIsBiphasic(int chan) const;
    float getPhase1Duration(int chan) const;
    float getPhase2Duration(int chan) const;
    float getInterPhaseInt(int chan) const;
    float getVoltage1(int chan) const;
    float getVoltage2(int chan) const;
    float getRestingVoltage(int chan) const;
    float getInterPulseInt(int chan) const;
    float getBurstDuration(int chan) const;
    float getInterBurstInt(int chan) const;
    float getTrainDuration(int chan) const;
    float getTrainDelay(int chan) const;
    int getLinkTriggerChannel1(int chan) const;
    int getLinkTriggerChannel2(int chan) const;
    int getTriggerMode(int chan) const;
    uint32_t getPulsePalVersion() const;

    void setIsBiphasic(int chan, bool isBiphasic);
    void setNegFirst(int chan, bool negFirst);
    void setPhase1Duration(int chan, float phaseDuration);
    void setPhase2Duration(int chan, float phaseDuration);
    void setInterPhaseInt(int chan, float interPhaseInt);
    void setVoltage1(int chan, float voltage);
    void setVoltage2(int chan, float voltage);
    void setRestingVoltage(int chan, float voltage);
    void setInterPulseInt(int chan, float interPulseInt);
    void setBurstDuration(int chan, float burstDuration);
    void setInterBurstInt(int chan, float interBurstInt);
    void setTrainDuration(int chan, float trainDuration);
    void setTrainDelay(int chan, float trainDelay);
    void setLinkTriggerChannel1(int chan, int link);
    void setLinkTriggerChannel2(int chan, int link);
    void setTriggerMode(int chan, int mode);
    void setTTLsettings(int chan);

    bool checkParameterConsistency(int chan);
    void adjustParameters(int chan);

    void addEventSource(EventSources s);
    void clearEventSources();

private:
    Array<int> channelTtlTrigger;
    Array<int> channelTtlGate;
    Array<bool> channelState;
    Array<EventSources> sources;

    int channelToChange;

    // PulsePal params
    vector<int> m_isBiphasic;
    vector<float> m_phase1Duration; // ms
    vector<float> m_phase2Duration; // ms
    vector<float> m_interPhaseInterval; // ms
    vector<float> m_trainDuration;
    vector<float> m_trainDelay;
    vector<float> m_burstDuration;
    vector<float> m_phase1Voltage; // V
    vector<float> m_phase2Voltage; // V
    vector<float> m_restingVoltage; // V
    vector<float> m_interPulseInterval; // ms
    vector<float> m_interBurstInterval; // ms
    vector<int> m_linkTriggerChannel1;
    vector<int> m_linkTriggerChannel2;
    vector<int> m_triggerMode;

    vector<int> m_repetitions;

    // PULSE PAL
    PulsePal pulsePal;
    uint32_t pulsePalVersion;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PulsePalOutput);
};


#endif  // __PULSEPALOUTPUT_H_A8BF66D6__
