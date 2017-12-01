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


#include "PulsePalOutputCanvas.h"
#include "PulsePalOutput.h"


PulsePalOutputCanvas::PulsePalOutputCanvas(PulsePalOutput* PulsePalOutput)
    : processor(PulsePalOutput)
    , labelColour(Colour(200, 255, 0))
    , labelTextColour(Colour(255, 200, 0))
    , labelBackgroundColour(Colour(100,100,100))

{
    // Setup buttons
    initButtons();
    // Setup Labels
    initLabels();

    addKeyListener(this);

    startCallbacks();
    update();
}

PulsePalOutputCanvas::~PulsePalOutputCanvas()
{
    TopLevelWindow::getTopLevelWindow(0)->removeKeyListener(this);
}

float PulsePalOutputCanvas::my_round(float x)
{
    return x < 0.0 ? ceil(x - 0.5) : floor(x + 0.5);
}


void PulsePalOutputCanvas::paint (Graphics& g)
{

    g.setColour(labelBackgroundColour); // backbackround color
    g.fillRect(0, 0, getWidth(), getHeight());


    // Check pulse Pal connection
    if (processor->getPulsePalVersion() > 0)
        pulsePalLabel->setText(String("Pulse Pal: ") +=  String("CONNECTED"), dontSendNotification);
    else
        pulsePalLabel->setText(String("Pulse Pal: ") +=  String("NOT CONNECTED"), dontSendNotification);

    for (int i; i < PULSEPALCHANNELS; i++)
    {
        g.setColour(Colours::grey);
        g.fillRoundedRectangle(0.005*getWidth() + 0.25*i*getWidth(), 0.15*getHeight(), 0.2*getWidth(), 0.7*getHeight(), 4.0f);
    }

}

void PulsePalOutputCanvas::resized()
{
    pulsePalLabel->setBounds(0.01*getWidth(), 0.05*getHeight(), 0.18*getWidth(),0.04*getHeight());

    for (int i; i < PULSEPALCHANNELS; i++)
    {
        channelLabel[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth(), 0.15*getHeight(), 0.08*getWidth(),0.04*getHeight());

        phase1Label[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth(), 0.25*getHeight(), 0.08*getWidth(),0.04*getHeight());
        phase1EditLabel[i]->setBounds(0.11*getWidth() + 0.25*i*getWidth(), 0.25*getHeight(), 0.08*getWidth(),0.04*getHeight());
        phase2Label[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth(), 0.3*getHeight(), 0.08*getWidth(),0.04*getHeight());
        phase2EditLabel[i]->setBounds(0.11*getWidth() + 0.25*i*getWidth(), 0.3*getHeight(), 0.08*getWidth(),0.04*getHeight());
        interphaseLabel[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth(), 0.35*getHeight(), 0.08*getWidth(),0.04*getHeight());
        interphaseEditLabel[i]->setBounds(0.11*getWidth() + 0.25*i*getWidth(), 0.35*getHeight(), 0.08*getWidth(),0.04*getHeight());
        voltage1Label[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth(), 0.4*getHeight(), 0.08*getWidth(),0.04*getHeight());
        voltage1EditLabel[i]->setBounds(0.11*getWidth() + 0.25*i*getWidth(), 0.4*getHeight(), 0.08*getWidth(),0.04*getHeight());
        voltage2Label[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth(), 0.45*getHeight(), 0.08*getWidth(),0.04*getHeight());
        voltage2EditLabel[i]->setBounds(0.11*getWidth() + 0.25*i*getWidth(), 0.45*getHeight(), 0.08*getWidth(),0.04*getHeight());
        interpulseLabel[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth(), 0.5*getHeight(), 0.08*getWidth(),0.04*getHeight());
        interpulseEditLabel[i]->setBounds(0.11*getWidth() + 0.25*i*getWidth(), 0.5*getHeight(), 0.08*getWidth(),0.04*getHeight());
        repetitionsLabel[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth(), 0.55*getHeight(), 0.08*getWidth(),0.04*getHeight());
        repetitionsEditLabel[i]->setBounds(0.11*getWidth() + 0.25*i*getWidth(), 0.55*getHeight(), 0.08*getWidth(),0.04*getHeight());
        trainDurationLabel[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth(), 0.6*getHeight(), 0.08*getWidth(),0.04*getHeight());
        trainDurationEditLabel[i]->setBounds(0.11*getWidth() + 0.25*i*getWidth(), 0.6*getHeight(), 0.08*getWidth(),0.04*getHeight());

        ttlButton[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth(), 0.68*getHeight(), 0.18*getWidth(), 0.06*getHeight());
        biphasicButton[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth(), 0.76*getHeight(), 0.18*getWidth(), 0.03*getHeight());
        negFirstButton[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth(), 0.8*getHeight(), 0.09*getWidth(),0.03*getHeight());
        posFirstButton[i]->setBounds(0.01*getWidth() + 0.25*i*getWidth() + 0.09*getWidth(), 0.8*getHeight(), 0.09*getWidth(),0.03*getHeight());

    }

    refresh();
}

void PulsePalOutputCanvas::buttonClicked(Button* button)
{
    //    if (button == biphasicButton)
    //    {
    //        if (button->getToggleState()==true)
    //            processor->setIsBiphasic(processor->getChan(), true);
    //        else
    //            processor->setIsBiphasic(processor->getChan(), false);

    //        // update train duration/repetition
    //        if (!processor->checkParameterConsistency(processor->getChan()))
    //        {
    //            trainDurationEditLabel->setText(String(processor->getTrainDuration(processor->getChan())), dontSendNotification);
    //            interpulseEditLabel->setText(String(processor->getInterPulseInt(processor->getChan())), dontSendNotification);
    //            processor->setRepetitions(processor->getChan(), 1);
    //            repetitionsEditLabel->setText(String(1), dontSendNotification);
    //        }
    //        processor->setRepetitionsTrainDuration(processor->getChan(), processor->REPFIRST);
    ////        uploadInfoOnLoad();
    //    }
    //    else if (button == negFirstButton)
    //    {
    //        if (button->getToggleState()==true){
    //            if (posFirstButton->getToggleState()==true)
    //                posFirstButton->triggerClick();

    //            processor->setNegFirst(processor->getChan(), true);
    //        }
    //        else
    //            if (posFirstButton->getToggleState()==false)
    //                posFirstButton->triggerClick();
    //    }
    //    else if (button == posFirstButton)
    //    {
    //        if (button->getToggleState()==true){
    //            if (negFirstButton->getToggleState()==true)
    //                negFirstButton->triggerClick();

    //            processor->setNegFirst(processor->getChan(), false);
    //        }
    //        else
    //            if (negFirstButton->getToggleState()==false)
    //                negFirstButton->triggerClick();
    //    }
    //    else if (button == chan1Button)
    //    {
    //        if (button->getToggleState()==true){
    //            if (chan2Button->getToggleState()==true)
    //                chan2Button->triggerClick();
    //            if (chan3Button->getToggleState()==true)
    //                chan3Button->triggerClick();
    //            if (chan4Button->getToggleState()==true)
    //                chan4Button->triggerClick();

    //            processor->setChan(0);

    //            // retrieve and set label values

    //            phaseEditLabel->setText(String(processor->getPhaseDuration(0)), dontSendNotification);
    //            interphaseEditLabel->setText(String(processor->getInterPhaseInt(0)), dontSendNotification);
    //            voltageEditLabel->setText(String(processor->getVoltage(0)), dontSendNotification);
    //            repetitionsEditLabel->setText(String(processor->getRepetitions(0)), dontSendNotification);
    //            interpulseEditLabel->setText(String(processor->getInterPulseInt(0)), dontSendNotification);



    //            if (processor->getIsBiphasic(0) == true) {
    //                if (biphasicButton->getToggleState()==false)
    //                    biphasicButton->triggerClick();
    //            } else {
    //                if (biphasicButton->getToggleState()==true)
    //                    biphasicButton->triggerClick();
    //            }

    //            if (processor->getNegFirst(0) == true) {
    //                if (negFirstButton->getToggleState()==false)
    //                    negFirstButton->triggerClick();
    //            } else {
    //                if (negFirstButton->getToggleState()==true)
    //                    negFirstButton->triggerClick();
    //            }


    //        }
    //        else
    //            if (chan1Button->getToggleState()==false && chan2Button->getToggleState() == false
    //                    && chan3Button->getToggleState()==false && chan4Button->getToggleState() == false)
    //                chan1Button->triggerClick();
    //    }
    //    else if (button == chan2Button)
    //    {
    //        if (button->getToggleState()==true){
    //            if (chan1Button->getToggleState()==true)
    //                chan1Button->triggerClick();
    //            if (chan3Button->getToggleState()==true)
    //                chan3Button->triggerClick();
    //            if (chan4Button->getToggleState()==true)
    //                chan4Button->triggerClick();

    //            processor->setChan(1);

    //            // retrieve and set label values

    //            phaseEditLabel->setText(String(processor->getPhaseDuration(1)), dontSendNotification);
    //            interphaseEditLabel->setText(String(processor->getInterPhaseInt(1)), dontSendNotification);
    //            voltageEditLabel->setText(String(processor->getVoltage(1)), dontSendNotification);
    //            repetitionsEditLabel->setText(String(processor->getRepetitions(1)), dontSendNotification);
    //            interpulseEditLabel->setText(String(processor->getInterPulseInt(1)), dontSendNotification);



    //            if (processor->getIsBiphasic(1) == true) {
    //                if (biphasicButton->getToggleState()==false)
    //                    biphasicButton->triggerClick();
    //            } else {
    //                if (biphasicButton->getToggleState()==true)
    //                    biphasicButton->triggerClick();
    //            }

    //            if (processor->getNegFirst(1) == true) {
    //                if (negFirstButton->getToggleState()==false)
    //                    negFirstButton->triggerClick();
    //            } else {
    //                if (negFirstButton->getToggleState()==true)
    //                    negFirstButton->triggerClick();
    //            }


    //        }
    //        else
    //            if (chan1Button->getToggleState()==false && chan2Button->getToggleState() == false
    //                    && chan3Button->getToggleState()==false && chan4Button->getToggleState() == false)
    //                chan2Button->triggerClick();

    //    }
    //    else if (button == chan3Button)
    //    {
    //        if (button->getToggleState()==true){
    //            if (chan2Button->getToggleState()==true)
    //                chan2Button->triggerClick();
    //            if (chan1Button->getToggleState()==true)
    //                chan1Button->triggerClick();
    //            if (chan4Button->getToggleState()==true)
    //                chan4Button->triggerClick();

    //            processor->setChan(2);


    //            phaseEditLabel->setText(String(processor->getPhaseDuration(2)), dontSendNotification);
    //            interphaseEditLabel->setText(String(processor->getInterPhaseInt(2)), dontSendNotification);
    //            voltageEditLabel->setText(String(processor->getVoltage(2)), dontSendNotification);
    //            repetitionsEditLabel->setText(String(processor->getRepetitions(2)), dontSendNotification);
    //            interpulseEditLabel->setText(String(processor->getInterPulseInt(2)), dontSendNotification);



    //            if (processor->getIsBiphasic(2) == true) {
    //                if (biphasicButton->getToggleState()==false)
    //                    biphasicButton->triggerClick();
    //            } else {
    //                if (biphasicButton->getToggleState()==true)
    //                    biphasicButton->triggerClick();
    //            }

    //            if (processor->getNegFirst(2) == true) {
    //                if (negFirstButton->getToggleState()==false)
    //                    negFirstButton->triggerClick();
    //            } else {
    //                if (negFirstButton->getToggleState()==true)
    //                    negFirstButton->triggerClick();
    //            }
    //        }
    //        else
    //            if (chan1Button->getToggleState()==false && chan2Button->getToggleState() == false
    //                    && chan3Button->getToggleState()==false && chan4Button->getToggleState() == false)
    //                chan3Button->triggerClick();

    //    }
    //    else if (button == chan4Button)
    //    {
    //        if (button->getToggleState()==true){
    //            if (chan2Button->getToggleState()==true)
    //                chan2Button->triggerClick();
    //            if (chan3Button->getToggleState()==true)
    //                chan3Button->triggerClick();
    //            if (chan1Button->getToggleState()==true)
    //                chan1Button->triggerClick();

    //            processor->setChan(3);


    //            phaseEditLabel->setText(String(processor->getPhaseDuration(3)), dontSendNotification);
    //            interphaseEditLabel->setText(String(processor->getInterPhaseInt(3)), dontSendNotification);
    //            voltageEditLabel->setText(String(processor->getVoltage(3)), dontSendNotification);
    //            repetitionsEditLabel->setText(String(processor->getRepetitions(3)), dontSendNotification);
    //            interpulseEditLabel->setText(String(processor->getInterPulseInt(3)), dontSendNotification);



    //            if (processor->getIsBiphasic(3) == true) {
    //                if (biphasicButton->getToggleState()==false)
    //                    biphasicButton->triggerClick();
    //            } else {
    //                if (biphasicButton->getToggleState()==true)
    //                    biphasicButton->triggerClick();
    //            }

    //            if (processor->getNegFirst(3) == true) {
    //                if (negFirstButton->getToggleState()==false)
    //                    negFirstButton->triggerClick();
    //            } else {
    //                if (negFirstButton->getToggleState()==true)
    //                    negFirstButton->triggerClick();
    //            }
    //        }
    //        else
    //            if (chan1Button->getToggleState()==false && chan2Button->getToggleState() == false
    //                    && chan3Button->getToggleState()==false && chan4Button->getToggleState() == false)
    //                chan4Button->triggerClick();

    //    }

    processor->updatePulsePal();
    repaint();

}

bool PulsePalOutputCanvas::keyPressed(const KeyPress &key, Component *originatingComponent)
{

}

void PulsePalOutputCanvas::labelTextChanged(Label *label)
{
    //    if (label == phaseEditLabel)
    //    {
    //        // 100 - 3600*10e3 (3600 s)
    //        Value val = label->getTextValue();
    //        float value = float(my_round(float(val.getValue())*10) / 10); //only multiple of 100us
    //        if ((float(val.getValue())>=0 && int(val.getValue())<=3600*10e3))
    //        {
    //            processor->setPhaseDuration(processor->getChan(), value);
    //            label->setText(String(value), dontSendNotification);
    //            if (!processor->checkParameterConsistency(processor->getChan()))
    //            {
    //                CoreServices::sendStatusMessage("Inconsistent Values!");
    //                //if values are inconsestent here phase is  > interpulse or trainduration
    //                processor->setRepetitions(processor->getChan(), 1);
    //                repetitionsEditLabel->setText(String(1), dontSendNotification);
    //            }
    //            trainDurationEditLabel->setText(String(processor->getTrainDuration(processor->getChan())), dontSendNotification);
    //            interpulseEditLabel->setText(String(processor->getInterPulseInt(processor->getChan())), dontSendNotification);
    //        }
    //        else
    //        {
    //            CoreServices::sendStatusMessage("Selected values must be within 0 and 3600*10e3 with 0.1 step!");
    //            label->setText("", dontSendNotification);

    //        }
    //    }
    //    if (label == interphaseEditLabel)
    //    {
    //        Value val = label->getTextValue();
    //        float value = float(my_round(float(val.getValue())*10) / 10); //only multiple of 100us
    //        if ((float(val.getValue())>=0 && int(val.getValue())<=3600*10e3))
    //        {
    //            processor->setInterPhaseInt(processor->getChan(), value);
    //            label->setText(String(value), dontSendNotification);
    //            if (!processor->checkParameterConsistency(processor->getChan()))
    //            {
    //                CoreServices::sendStatusMessage("Inconsistent Values!");
    //                processor->setInterPhaseInt(processor->getChan(), DEF_INTER_PHASE);
    //                label->setText("", dontSendNotification);
    //            }
    //        }
    //        else
    //        {
    //            CoreServices::sendStatusMessage("Selected values must be within 0 and 3600*10e3 with 0.1 step!");
    //            label->setText("", dontSendNotification);
    //        }
    //    }
    //    if (label == voltageEditLabel)
    //    {
    //        Value val = label->getTextValue();
    //        if (float(val.getValue())>=0 && float(val.getValue())<=10)
    //        {
    //            //            float value = float(int(float(val.getValue())*100) / 5 * 5)/100; //only multiple of 100us
    //            processor->setVoltage(processor->getChan(), float(val.getValue()));
    //        }
    //        else
    //        {
    //            CoreServices::sendStatusMessage("Selected values must be within 0 and 10 with 0.05 step!");
    //            label->setText("", dontSendNotification);
    //        }
    //    }
    //    if (label == repetitionsEditLabel)
    //    {
    //        Value val = label->getTextValue();
    //        if (int(val.getValue())>=1)
    //        {
    //            processor->setRepetitions(processor->getChan(), int(val.getValue()));
    //            processor->setRepetitionsTrainDuration(processor->getChan(), processor->REPFIRST);
    //            trainDurationEditLabel->setText(String(processor->getTrainDuration(processor->getChan())), dontSendNotification);
    //            if (!processor->checkParameterConsistency(processor->getChan()))
    //            {
    //                CoreServices::sendStatusMessage("Inconsistent Values!");
    //                processor->setRepetitions(processor->getChan(), DEF_REPETITIONS);
    //                label->setText("", dontSendNotification);
    //            }
    //        }
    //        else
    //        {
    //            CoreServices::sendStatusMessage("Selected values cannot be less than 1!");
    //            label->setText("", dontSendNotification);
    //        }
    //    }
    //    if (label == interpulseEditLabel)
    //    {
    //        Value val = label->getTextValue();
    //        float value = float(my_round(float(val.getValue())*10) / 10); //only multiple of 100us
    //        if ((float(val.getValue())>=0 && int(val.getValue())<=3600*10e3))
    //        {
    //            processor->setInterPulseInt(processor->getChan(), value);
    //            label->setText(String(value), dontSendNotification);
    //            processor->setRepetitionsTrainDuration(processor->getChan(), processor->REPFIRST);
    //            trainDurationEditLabel->setText(String(processor->getTrainDuration(processor->getChan())), dontSendNotification);
    //            if (!processor->checkParameterConsistency(processor->getChan()))
    //            {
    //                CoreServices::sendStatusMessage("Inconsistent Values!");
    //                processor->setInterPulseInt(processor->getChan(), DEF_INTER_PULSE);
    //                label->setText("", dontSendNotification);
    //            }
    //        }
    //        else
    //        {
    //            CoreServices::sendStatusMessage("Selected values must be within 0 and 3600*10e3 with 0.1 step!");
    //            label->setText("", dontSendNotification);
    //        }
    //    }
    //    if (label == trainDurationEditLabel)
    //    {
    //        Value val = label->getTextValue();
    //        float value = float(my_round(float(val.getValue())*10) / 10); //only multiple of 100us
    //        if (int(val.getValue())>=1)
    //        {
    //            processor->setTrainDuration(processor->getChan(), value);

    //            processor->setRepetitionsTrainDuration(processor->getChan(), processor->TRAINFIRST);
    //            repetitionsEditLabel->setText(String(processor->getRepetitions(processor->getChan())), dontSendNotification);
    //            if (!processor->checkParameterConsistency(processor->getChan()))
    //            {
    //                CoreServices::sendStatusMessage("Inconsistent Values!");
    //                //processor->setTrainDuration(processor->getChan(), DEF_TRAINDURATION);
    //                label->setText("", dontSendNotification);
    //            }
    //            label->setText(String(processor->getTrainDuration(processor->getChan())), dontSendNotification);
    //        }
    //        else
    //        {
    //            CoreServices::sendStatusMessage("Selected values cannot be less than 1!");
    //            label->setText("", dontSendNotification);
    //        }
    //    }
    for (int i; i < 4; i++)
    {
        if (label == phase1EditLabel[i])
            std::cout << i << " label changed" << std::endl;
    }

    processor->updatePulsePal();
}

void PulsePalOutputCanvas::initButtons()
{

    for (int i; i < PULSEPALCHANNELS; i++)
    {
        ScopedPointer<UtilityButton> biph = new UtilityButton("biphasic", Font("Small Text", 20, Font::plain));
        biph->setRadius(3.0f);
        biph->addListener(this);
        biph->setClickingTogglesState(true);
        //    biphasicButton->triggerClick();
        biphasicButton[i] = biph;
        addAndMakeVisible(biphasicButton[i]);

        ScopedPointer<UtilityButton> neg = new UtilityButton("neg", Font("Small Text", 20, Font::plain));
        neg->setRadius(3.0f);
        neg->addListener(this);
        neg->setClickingTogglesState(true);
        //    negFirstButton->triggerClick();
        negFirstButton[i] = neg;
        addAndMakeVisible(negFirstButton[i]);

        ScopedPointer<UtilityButton> pos = new UtilityButton("pos", Font("Small Text", 20, Font::plain));
        pos->setRadius(3.0f);
        pos->addListener(this);
        pos->setClickingTogglesState(true);
        posFirstButton[i] = pos;
        addAndMakeVisible(posFirstButton[i]);

        ScopedPointer<UtilityButton> ttl = new UtilityButton("ttl", Font("Small Text", 25, Font::plain));
        ttl->setRadius(3.0f);
        ttl->addListener(this);
        ttl->setClickingTogglesState(true);
        ttlButton[i] = ttl;
        addAndMakeVisible(ttlButton[i]);

        //    chan1Button = new UtilityButton("1", Font("Small Text", 13, Font::plain));
        //    chan1Button->setRadius(3.0f);
        //    chan1Button->addListener(this);
        //    chan1Button->setClickingTogglesState(true);
        //    chan1Button->triggerClick();
        //    addAndMakeVisible(chan1Button);

        //    chan2Button = new UtilityButton("2", Font("Small Text", 13, Font::plain));
        //    chan2Button->setRadius(3.0f);
        //    chan2Button->addListener(this);
        //    chan2Button->setClickingTogglesState(true);
        //    addAndMakeVisible(chan2Button);

        //    chan3Button = new UtilityButton("3", Font("Small Text", 13, Font::plain));
        //    chan3Button->setRadius(3.0f);
        //    chan3Button->addListener(this);
        //    chan3Button->setClickingTogglesState(true);
        //    addAndMakeVisible(chan3Button);

        //    chan4Button = new UtilityButton("4", Font("Small Text", 13, Font::plain));
        //    chan4Button->setRadius(3.0f);
        //    chan4Button->addListener(this);
        //    chan4Button->setClickingTogglesState(true);
        //    addAndMakeVisible(chan4Button);

        //    if (processor->getIsBiphasic(0))
        //        biphasicButton->triggerClick();
        //    if (processor->getNegFirst(0))
        //        negFirstButton->triggerClick();
        //    else
        //        posFirstButton->triggerClick();
        //    }


    }
}

void PulsePalOutputCanvas::initLabels()
{
    pulsePalLabel = new Label("s_pulsePal", "Pulse Pal Status: ");
    pulsePalLabel->setFont(Font(40));
    pulsePalLabel->setColour(Label::textColourId, labelColour);
    addAndMakeVisible(pulsePalLabel);

    pulsePalLabel = new Label("s_pp", "Pulse Pal:");
    pulsePalLabel->setFont(Font(40));
    pulsePalLabel->setColour(Label::textColourId, labelColour);
    addAndMakeVisible(pulsePalLabel);


    for (int i; i < PULSEPALCHANNELS; i++)
    {
        ScopedPointer<Label> chan = new Label("s_phase1", "Channel "+String(i+1));
        chan->setFont(Font(30));
        chan->setColour(Label::textColourId, labelColour);
        channelLabel[i] = chan;
        addAndMakeVisible(channelLabel[i]);

        ScopedPointer<Label> ph1 = new Label("s_phase1", "phase1 [ms]:");
        ph1->setFont(Font(20));
        ph1->setColour(Label::textColourId, labelColour);
        phase1Label[i] = ph1;
        addAndMakeVisible(phase1Label[i]);

        ScopedPointer<Label> ph2 = new Label("s_phase2", "phase2 [ms]:");
        ph2->setFont(Font(20));
        ph2->setColour(Label::textColourId, labelColour);
        phase2Label[i] = ph2;
        addAndMakeVisible(phase2Label[i]);

        ScopedPointer<Label> intph = new Label("s_interphase", "interphase [ms]:");
        intph->setFont(Font(20));
        intph->setColour(Label::textColourId, labelColour);
        interphaseLabel[i] = intph;
        addAndMakeVisible(interphaseLabel[i]);

        ScopedPointer<Label> v1 = new Label("s_v1", "voltage1 [V]:");
        v1->setFont(Font(20));
        v1->setColour(Label::textColourId, labelColour);
        voltage1Label[i] = v1;
        addAndMakeVisible(voltage1Label[i]);

        ScopedPointer<Label> v2 = new Label("s_v2", "voltage2 [V]:");
        v2->setFont(Font(20));
        v2->setColour(Label::textColourId, labelColour);
        voltage2Label[i] = v2;
        addAndMakeVisible(voltage2Label[i]);

        ScopedPointer<Label> intpul = new Label("s_intpul", "interpulse [ms]:");
        intpul->setFont(Font(20));
        intpul->setColour(Label::textColourId, labelColour);
        interpulseLabel[i] = intpul;
        addAndMakeVisible(interpulseLabel[i]);

        ScopedPointer<Label> rep = new Label("s_rep", "repetitions:");
        rep->setFont(Font(20));
        rep->setColour(Label::textColourId, labelColour);
        repetitionsLabel[i] = rep;
        addAndMakeVisible(repetitionsLabel[i]);

        ScopedPointer<Label> train = new Label("s_train", "train duration [ms]:");
        train->setFont(Font(20));
        train->setColour(Label::textColourId, labelColour);
        trainDurationLabel[i] = train;
        addAndMakeVisible(trainDurationLabel[i]);

        ScopedPointer<Label> phEd1 = new Label("phase1", String(DEF_PHASE_DURATION));
        phEd1->setFont(Font(20));
        phEd1->setColour(Label::textColourId, labelTextColour);
        phEd1->setColour(Label::backgroundColourId, labelBackgroundColour);
        phEd1->setEditable(true);
        phase1EditLabel[i] = phEd1;
        phase1EditLabel[i]->addListener(this);
        addAndMakeVisible(phase1EditLabel[i]);

        ScopedPointer<Label> phEd2 = new Label("phase2", String(DEF_PHASE_DURATION));
        phEd2->setFont(Font(20));
        phEd2->setColour(Label::textColourId, labelTextColour);
        phEd2->setColour(Label::backgroundColourId, labelBackgroundColour);
        phEd2->setEditable(true);
        phase2EditLabel[i] = phEd2;
        phase2EditLabel[i]->addListener(this);
        addAndMakeVisible(phase2EditLabel[i]);

        ScopedPointer<Label> intphEd = new Label("interphase", String(DEF_INTER_PHASE));
        intphEd->setFont(Font(20));
        intphEd->setColour(Label::textColourId, labelTextColour);
        intphEd->setColour(Label::backgroundColourId, labelBackgroundColour);
        intphEd->setEditable(true);
        interphaseEditLabel[i] = intphEd;
        interphaseEditLabel[i]->addListener(this);
        addAndMakeVisible(interphaseEditLabel[i]);

        ScopedPointer<Label> vEd1 = new Label("v1", String(DEF_VOLTAGE));
        vEd1->setFont(Font(20));
        vEd1->setColour(Label::textColourId, labelTextColour);
        vEd1->setColour(Label::backgroundColourId, labelBackgroundColour);
        vEd1->setEditable(true);
        voltage1EditLabel[i] = vEd1;
        voltage1EditLabel[i]->addListener(this);
        addAndMakeVisible(voltage1EditLabel[i]);

        ScopedPointer<Label> vEd2 = new Label("v2", String(DEF_VOLTAGE));
        vEd2->setFont(Font(20));
        vEd2->setColour(Label::textColourId, labelTextColour);
        vEd2->setColour(Label::backgroundColourId, labelBackgroundColour);
        vEd2->setEditable(true);
        voltage2EditLabel[i] = vEd2;
        voltage2EditLabel[i]->addListener(this);
        addAndMakeVisible(voltage2EditLabel[i]);

        ScopedPointer<Label> intpulEd = new Label("pul", String(DEF_INTER_PULSE));
        intpulEd->setFont(Font(20));
        intpulEd->setColour(Label::textColourId, labelTextColour);
        intpulEd->setColour(Label::backgroundColourId, labelBackgroundColour);
        intpulEd->setEditable(true);
        interpulseEditLabel[i] = intpulEd;
        interpulseEditLabel[i]->addListener(this);
        addAndMakeVisible(interpulseEditLabel[i]);

        ScopedPointer<Label> repEd = new Label("rep", String(DEF_REPETITIONS));
        repEd->setFont(Font(20));
        repEd->setColour(Label::textColourId, labelTextColour);
        repEd->setColour(Label::backgroundColourId, labelBackgroundColour);
        repEd->setEditable(true);
        repetitionsEditLabel[i] = repEd;
        repetitionsEditLabel[i]->addListener(this);
        addAndMakeVisible(repetitionsEditLabel[i]);

        ScopedPointer<Label> trainEd = new Label("train", String(DEF_TRAINDURATION));
        trainEd->setFont(Font(20));
        trainEd->setColour(Label::textColourId, labelTextColour);
        trainEd->setColour(Label::backgroundColourId, labelBackgroundColour);
        trainEd->setEditable(true);
        trainDurationEditLabel[i] = trainEd;
        trainDurationEditLabel[i]->addListener(this);
        addAndMakeVisible(trainDurationEditLabel[i]);

    }

}


void PulsePalOutputCanvas::refreshState()
{
}

void PulsePalOutputCanvas::refresh()
{
}

void PulsePalOutputCanvas::beginAnimation()
{
}

void PulsePalOutputCanvas::endAnimation()
{
}

void PulsePalOutputCanvas::update()
{
    //    availableChans->clear();
    //    int nSources = processor->getNSources();
    //    int nextItem = 2;
    //    availableChans->addItem("None", 1);
    //    for (int i = 0; i < nSources; i++)
    //    {
    //        TrackingSources& source = processor->getTrackingSource(i);
    //        String name = String(source.sourceId) + " " + source.color;
    //        availableChans->addItem(name, nextItem++);
    //    }

}

void PulsePalOutputCanvas::setParameter(int, float)
{
}

void PulsePalOutputCanvas::setParameter(int, int, int, float)
{
}
