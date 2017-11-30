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


#include <stdio.h>

#include "PulsePalOutput.h"


PulsePalOutput::PulsePalOutput()
    : GenericProcessor ("Pulse Pal New")
    , channelToChange (0)
    , m_tot_chan (4)
{
    setProcessorType (PROCESSOR_TYPE_SINK);


    // Init PulsePal
    pulsePal.initialize();
    pulsePal.setDefaultParameters();
    pulsePal.updateDisplay("GUI Connected","Click for menu");
    pulsePalVersion = pulsePal.getFirmwareVersion();


    m_phaseDuration = vector<float>(m_tot_chan, DEF_PHASE_DURATION);
    m_interPhaseInt = vector<float>(m_tot_chan, DEF_INTER_PHASE);
    m_repetitions = vector<int>(m_tot_chan, DEF_REPETITIONS);
    m_voltage = vector<float>(m_tot_chan, DEF_VOLTAGE);
    m_interPulseInt = vector<float>(m_tot_chan, DEF_INTER_PULSE);
    m_trainDuration = vector<float>(m_tot_chan, DEF_TRAINDURATION);

    m_isBiphasic = vector<int>(m_tot_chan, 1);
    m_negativeFirst = vector<int>(m_tot_chan, 1);

    for (int i = 0; i < 4; ++i)
    {
        channelTtlTrigger.add   (-1);
        channelTtlGate.add      (-1);
        channelState.add        (true);
    }
}


PulsePalOutput::~PulsePalOutput()
{
    pulsePal.updateDisplay ("PULSE PAL v1.0","Click for menu");
}


AudioProcessorEditor* PulsePalOutput::createEditor()
{
    editor = new PulsePalOutputEditor (this, &pulsePal, true);
    return editor;
}


void PulsePalOutput::handleEvent (const EventChannel* eventInfo, const MidiMessage& event, int sampleNum)
{
    if (Event::getEventType(event) == EventChannel::TTL)
    {
        //  std::cout << "Received an event!" << std::endl;

		TTLEventPtr ttl = TTLEvent::deserializeFromMessage(event, eventInfo);

        // int eventNodeId = *(dataptr+1);
        const int eventId       = ttl->getState() ? 1 : 0;
        const int eventChannel  = ttl->getChannel();

        for (int i = 0; i < channelTtlTrigger.size(); ++i)
        {
            if (eventId == 1
                && eventChannel == channelTtlTrigger[i]
                && channelState[i])
            {
                pulsePal.triggerChannel (i + 1);
            }

            if (eventChannel == channelTtlGate[i])
            {
                if (eventId == 1)
                    channelState.set (i, true);
                else
                    channelState.set (i, false);
            }
        }
    }
}


void PulsePalOutput::setParameter (int parameterIndex, float newValue)
{
    editor->updateParameterButtons (parameterIndex);
    //std::cout << "Changing channel " << parameterIndex << " to " << newValue << std::endl;

    switch (parameterIndex)
    {
        case 0:
            channelToChange = (int) newValue - 1;
            break;

        case 1:
            channelTtlTrigger.set (channelToChange, (int) newValue);
            break;

        case 2:
            channelTtlGate.set (channelToChange, (int) newValue);

            if (newValue < 0)
            {
                channelState.set (channelToChange, true);
            }
            else
            {
                channelState.set (channelToChange, false);
            }
            break;

        default:
            std::cout << "Unrecognized parameter index." << std::endl;
    }
}


void PulsePalOutput::process (AudioSampleBuffer& buffer)
{
    checkForEvents ();
}

bool PulsePalOutput::updatePulsePal()
{
    // check that Pulspal is connected and update param
    if (pulsePalVersion != 0)
    {
        int actual_chan = m_chan+1;
        float pulse_duration = 0;
        pulsePal.setBiphasic(actual_chan, m_isBiphasic[m_chan]);
        if (m_isBiphasic[m_chan])
        {
            //pulse_duration = float(m_phaseDuration[m_chan])/1000*2 + float(m_interPhaseInt[m_chan])/1000;
            pulsePal.setPhase1Duration(actual_chan, float(m_phaseDuration[m_chan])/1000);
            pulsePal.setPhase2Duration(actual_chan, float(m_phaseDuration[m_chan])/1000);
            pulsePal.setInterPhaseInterval(actual_chan, float(m_interPhaseInt[m_chan])/1000);
            if (m_negativeFirst[m_chan])
            {
                pulsePal.setPhase1Voltage(actual_chan, - m_voltage[m_chan]);
                pulsePal.setPhase2Voltage(actual_chan, m_voltage[m_chan]);
            }
            else
            {
                pulsePal.setPhase1Voltage(actual_chan, m_voltage[m_chan]);
                pulsePal.setPhase2Voltage(actual_chan, - m_voltage[m_chan]);
            }
        }
        else
        {
            pulse_duration = float(m_phaseDuration[m_chan])/1000;
            pulsePal.setPhase1Duration(actual_chan, float(m_phaseDuration[m_chan])/1000);
            pulsePal.setPhase2Duration(actual_chan, 0);
            pulsePal.setInterPhaseInterval(actual_chan, 0);
            if (m_negativeFirst[m_chan])
            {
                pulsePal.setPhase1Voltage(actual_chan, - m_voltage[m_chan]);
            }
            else
            {
                pulsePal.setPhase1Voltage(actual_chan, m_voltage[m_chan]);
            }

        }

        //float train_duration = float(m_interPulseInt[m_chan])/1000 * m_repetitions[m_chan] + float(m_phaseDuration[m_chan])/1000;
        pulsePal.setPulseTrainDuration(actual_chan, float(m_trainDuration[m_chan])/1000);

        pulsePal.setInterPulseInterval(actual_chan, float(m_interPulseInt[m_chan])/1000);

        return true;
    }
    else
        return false;
}

bool PulsePalOutput::getIsBiphasic(int chan) const
{
    if (m_isBiphasic[chan])
        return true;
    else
        return false;
}
bool PulsePalOutput::getNegFirst(int chan) const
{
    if (m_negativeFirst[chan])
        return true;
    else
        return false;
}
float PulsePalOutput::getPhaseDuration(int chan) const
{
    return m_phaseDuration[chan];
}
float PulsePalOutput::getInterPhaseInt(int chan) const
{
    return m_interPhaseInt[chan];
}
float PulsePalOutput::getVoltage(int chan) const
{
    return m_voltage[chan];
}
int PulsePalOutput::getRepetitions(int chan) const
{
    return m_repetitions[chan];
}
float PulsePalOutput::getInterPulseInt(int chan) const
{
    return m_interPulseInt[chan];
}
float PulsePalOutput::getTrainDuration(int chan) const
{
    return m_trainDuration[chan];
}

uint32_t PulsePalOutput::getPulsePalVersion() const
{
    return pulsePalVersion;
}


void PulsePalOutput::setIsBiphasic(int chan, bool isBiphasic)
{
    if (isBiphasic)
        m_isBiphasic[chan] = 1;
    else
        m_isBiphasic[chan] = 0;
}
void PulsePalOutput::setNegFirst(int chan, bool negFirst)
{
    if (negFirst)
        m_negativeFirst[chan] = 1;
    else
        m_negativeFirst[chan] = 0;

}
void PulsePalOutput::setPhaseDuration(int chan, float phaseDuration)
{
    m_phaseDuration[chan] = phaseDuration;
    //    updatePulsePal();
}
void PulsePalOutput::setInterPhaseInt(int chan, float interPhaseInt)
{
    m_interPhaseInt[chan] = interPhaseInt;
    //    updatePulsePal();
}
void PulsePalOutput::setVoltage(int chan, float voltage)
{
    m_voltage[chan] = voltage;
    //    updatePulsePal();
}
void PulsePalOutput::setRepetitions(int chan, int rep)
{
    m_repetitions[chan] = rep;
    //    updatePulsePal();
}
void PulsePalOutput::setInterPulseInt(int chan, float interPulseInt)
{
    m_interPulseInt[chan] = interPulseInt;
    //    updatePulsePal();
}
void PulsePalOutput::setTrainDuration(int chan, float trainDuration)
{
    m_trainDuration[chan] = trainDuration;
    //    updatePulsePal();
}

void PulsePalOutput::setChan(int chan)
{
    m_chan = chan;
}


int PulsePalOutput::getChan() const
{
    return m_chan;
}

bool PulsePalOutput::checkParameterConsistency(int chan)
{
    if (m_repetitions[chan] > 1)
        if (!m_isBiphasic[chan])
            return !(m_phaseDuration[chan] + m_interPulseInt[chan] > m_trainDuration[chan]);
        else
            return !(2*m_phaseDuration[chan] + m_interPhaseInt[chan] + m_interPulseInt[chan] > m_trainDuration[chan]);
    else
    {
        if (!m_isBiphasic[chan])
        {
            //            m_interPulseInt[chan] = m_phaseDuration[chan];
            m_trainDuration[chan] = m_phaseDuration[chan];
        }
        else
        {
            //            m_interPulseInt[chan] = 2*m_phaseDuration[chan] + m_interPhaseInt[chan];
            m_trainDuration[chan] = 2*m_phaseDuration[chan] + m_interPhaseInt[chan];
        }
        return true;
    }
}

void PulsePalOutput::setRepetitionsTrainDuration(int chan, priority whatFirst)
{
    if (whatFirst == REPFIRST)
    {
        if (m_repetitions[chan] > 1)
            if (!m_isBiphasic[chan])
                m_trainDuration[chan] = (m_phaseDuration[chan] + m_interPulseInt[chan])*m_repetitions[chan]; // + m_phaseDuration[chan];
            else
                m_trainDuration[chan] = (2*m_phaseDuration[chan] + m_interPhaseInt[chan] + m_interPulseInt[chan])*m_repetitions[chan]; // + (2*m_phaseDuration[chan]+ m_interPhaseInt[chan]);
        else
            if (!m_isBiphasic[chan])
                m_trainDuration[chan] = m_phaseDuration[chan];
            else
                m_trainDuration[chan] = 2*m_phaseDuration[chan] + m_interPhaseInt[chan];
    }
    else
    {
        if (!m_isBiphasic[chan])
            if (int(m_trainDuration[chan]/(m_phaseDuration[chan] + m_interPulseInt[chan])) > 1)
                m_repetitions[chan] = int(m_trainDuration[chan]/(m_phaseDuration[chan] + m_interPulseInt[chan]));
            else
            {
                m_repetitions[chan] = 1;
                m_trainDuration[chan] = m_phaseDuration[chan];
            }
        else
            if (int(m_trainDuration[chan]/(2*m_phaseDuration[chan] + m_interPulseInt[chan] + m_interPulseInt[chan]) > 1))
                m_repetitions[chan] = int(m_trainDuration[chan]/(2*m_phaseDuration[chan] + m_interPulseInt[chan] + m_interPulseInt[chan]));
            else
            {
                m_repetitions[chan] = 1;
                m_trainDuration[chan] = 2*m_phaseDuration[chan] + m_interPhaseInt[chan];
            }
    }
}
