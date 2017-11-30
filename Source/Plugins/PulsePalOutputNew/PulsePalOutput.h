/*
    ------------------------------------------------------------------

    This file is part of the Open Ephys GUI
    Copyright (C) 2016 Open Ephys

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
#define DEF_TRAINDURATION 10
#define DEF_VOLTAGE 5

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

    enum priority {REPFIRST, TRAINFIRST};

    AudioProcessorEditor* createEditor() override;

    void process (AudioSampleBuffer& buffer) override;

    void setParameter (int parameterIndex, float newValue) override;

    void handleEvent (const EventChannel* eventInfo, const MidiMessage& event, int sampleNum) override;

    // Pulse Pal
    bool updatePulsePal();

    bool getIsBiphasic(int chan) const;
    bool getNegFirst(int chan) const;
    float getPhaseDuration(int chan) const;
    float getInterPhaseInt(int chan) const;
    float getVoltage(int chan) const;
    int getRepetitions(int chan) const;
    float getInterPulseInt(int chan) const;
    float getTrainDuration(int chan) const;
    uint32_t getPulsePalVersion() const;

    void setIsBiphasic(int chan, bool isBiphasic);
    void setNegFirst(int chan, bool negFirst);
    void setPhaseDuration(int chan, float phaseDuration);
    void setInterPhaseInt(int chan, float interPhaseInt);
    void setVoltage(int chan, float voltage);
    void setRepetitions(int chan, int rep);
    void setInterPulseInt(int chan, float interPulseInt);
    void setTrainDuration(int chan, float trainDuration);
    void setChan(int chan);
    void setTTLSyncChan(int chan);
    void setStimSyncChan(int chan);

    bool checkParameterConsistency(int chan);
    void setRepetitionsTrainDuration(int chan, priority whatFirst);

    int getChan() const;


private:
    Array<int> channelTtlTrigger;
    Array<int> channelTtlGate;
    Array<bool> channelState;
    int channelToChange;

    // Pulse params
    vector<int> m_isBiphasic;
    vector<int> m_negativeFirst;
    vector<float> m_phaseDuration; // ms
    vector<float> m_interPhaseInt; // ms
    vector<int> m_repetitions;
    vector<float> m_trainDuration;
    vector<float> m_voltage; // V
    vector<float> m_interPulseInt; // ms

    int m_chan;
    int m_tot_chan;

    // PULSE PAL
    PulsePal pulsePal;
    uint32_t pulsePalVersion;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PulsePalOutput);
};


#endif  // __PULSEPALOUTPUT_H_A8BF66D6__
