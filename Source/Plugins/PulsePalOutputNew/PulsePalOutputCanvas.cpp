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

    g.setColour(Colours::black); // backbackround color
    g.fillRect(0, 0, getWidth(), getHeight());


    // Check pulse Pal connection
    if (processor->getPulsePalVersion() > 0)
        pulsePalLabel->setText(String("Pulse Pal: ") +=  String("CONNECTED"), dontSendNotification);
    else
        pulsePalLabel->setText(String("Pulse Pal: ") +=  String("NOT CONNECTED"), dontSendNotification);

}

void PulsePalOutputCanvas::resized()
{

    // Express all positions as proportion of height and width
    // Buttons


    chan1Button->setBounds(getWidth() - 0.2*getWidth(), 0.32*getHeight(), 0.045*getWidth(),0.05*getHeight());
    chan2Button->setBounds(getWidth() - 0.2*getWidth() + 0.045*getWidth(), 0.32*getHeight(), 0.045*getWidth(),0.05*getHeight());
    chan3Button->setBounds(getWidth() - 0.2*getWidth() + 0.09 *getWidth(), 0.32*getHeight(), 0.045*getWidth(),0.05*getHeight());
    chan4Button->setBounds(getWidth() - 0.2*getWidth() + 0.135*getWidth(), 0.32*getHeight(), 0.045*getWidth(),0.05*getHeight());

    biphasicButton->setBounds(getWidth() - 0.2*getWidth(), 0.6*getHeight(), 0.18*getWidth(),0.03*getHeight());
    negFirstButton->setBounds(getWidth() - 0.2*getWidth(), 0.65*getHeight(), 0.09*getWidth(),0.03*getHeight());
    posFirstButton->setBounds(getWidth() - 0.2*getWidth() + 0.09*getWidth(), 0.65*getHeight(), 0.09*getWidth(),0.03*getHeight());



//    // Static Labels
    pulsePalLabel->setBounds(getWidth() - 0.4*getWidth(), 0.1*getHeight(), 0.18*getWidth(),0.04*getHeight());

    phaseLabel->setBounds(getWidth() - 0.2*getWidth(), 0.7*getHeight(), 0.1*getWidth(),0.04*getHeight());
    interphaseLabel->setBounds(getWidth() - 0.2*getWidth(), 0.75*getHeight(), 0.1*getWidth(),0.04*getHeight());
    voltageLabel->setBounds(getWidth() - 0.2*getWidth(), 0.8*getHeight(), 0.1*getWidth(),0.04*getHeight());
    interpulseLabel->setBounds(getWidth() - 0.2*getWidth(), 0.85*getHeight(), 0.1*getWidth(),0.04*getHeight());
    repetitionsLabel->setBounds(getWidth() - 0.2*getWidth(), 0.9*getHeight(), 0.1*getWidth(),0.04*getHeight());
    trainDurationLabel->setBounds(getWidth() - 0.2*getWidth(), 0.95*getHeight(), 0.1*getWidth(),0.04*getHeight());


    phaseEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.7*getHeight(), 0.08*getWidth(),0.04*getHeight());
    interphaseEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.75*getHeight(), 0.08*getWidth(),0.04*getHeight());
    voltageEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.8*getHeight(), 0.08*getWidth(),0.04*getHeight());
    interpulseEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.85*getHeight(), 0.08*getWidth(),0.04*getHeight());
    repetitionsEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.9*getHeight(), 0.08*getWidth(),0.04*getHeight());
    trainDurationEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.95*getHeight(), 0.08*getWidth(),0.04*getHeight());

    refresh();
}

void PulsePalOutputCanvas::buttonClicked(Button* button)
{
    if (button == biphasicButton)
    {
        if (button->getToggleState()==true)
            processor->setIsBiphasic(processor->getChan(), true);
        else
            processor->setIsBiphasic(processor->getChan(), false);

        // update train duration/repetition
        if (!processor->checkParameterConsistency(processor->getChan()))
        {
            trainDurationEditLabel->setText(String(processor->getTrainDuration(processor->getChan())), dontSendNotification);
            interpulseEditLabel->setText(String(processor->getInterPulseInt(processor->getChan())), dontSendNotification);
            processor->setRepetitions(processor->getChan(), 1);
            repetitionsEditLabel->setText(String(1), dontSendNotification);
        }
        processor->setRepetitionsTrainDuration(processor->getChan(), processor->REPFIRST);
//        uploadInfoOnLoad();
    }
    else if (button == negFirstButton)
    {
        if (button->getToggleState()==true){
            if (posFirstButton->getToggleState()==true)
                posFirstButton->triggerClick();

            processor->setNegFirst(processor->getChan(), true);
        }
        else
            if (posFirstButton->getToggleState()==false)
                posFirstButton->triggerClick();
    }
    else if (button == posFirstButton)
    {
        if (button->getToggleState()==true){
            if (negFirstButton->getToggleState()==true)
                negFirstButton->triggerClick();

            processor->setNegFirst(processor->getChan(), false);
        }
        else
            if (negFirstButton->getToggleState()==false)
                negFirstButton->triggerClick();
    }
    else if (button == chan1Button)
    {
        if (button->getToggleState()==true){
            if (chan2Button->getToggleState()==true)
                chan2Button->triggerClick();
            if (chan3Button->getToggleState()==true)
                chan3Button->triggerClick();
            if (chan4Button->getToggleState()==true)
                chan4Button->triggerClick();

            processor->setChan(0);

            // retrieve and set label values

            phaseEditLabel->setText(String(processor->getPhaseDuration(0)), dontSendNotification);
            interphaseEditLabel->setText(String(processor->getInterPhaseInt(0)), dontSendNotification);
            voltageEditLabel->setText(String(processor->getVoltage(0)), dontSendNotification);
            repetitionsEditLabel->setText(String(processor->getRepetitions(0)), dontSendNotification);
            interpulseEditLabel->setText(String(processor->getInterPulseInt(0)), dontSendNotification);



            if (processor->getIsBiphasic(0) == true) {
                if (biphasicButton->getToggleState()==false)
                    biphasicButton->triggerClick();
            } else {
                if (biphasicButton->getToggleState()==true)
                    biphasicButton->triggerClick();
            }

            if (processor->getNegFirst(0) == true) {
                if (negFirstButton->getToggleState()==false)
                    negFirstButton->triggerClick();
            } else {
                if (negFirstButton->getToggleState()==true)
                    negFirstButton->triggerClick();
            }


        }
        else
            if (chan1Button->getToggleState()==false && chan2Button->getToggleState() == false
                    && chan3Button->getToggleState()==false && chan4Button->getToggleState() == false)
                chan1Button->triggerClick();
    }
    else if (button == chan2Button)
    {
        if (button->getToggleState()==true){
            if (chan1Button->getToggleState()==true)
                chan1Button->triggerClick();
            if (chan3Button->getToggleState()==true)
                chan3Button->triggerClick();
            if (chan4Button->getToggleState()==true)
                chan4Button->triggerClick();

            processor->setChan(1);

            // retrieve and set label values

            phaseEditLabel->setText(String(processor->getPhaseDuration(1)), dontSendNotification);
            interphaseEditLabel->setText(String(processor->getInterPhaseInt(1)), dontSendNotification);
            voltageEditLabel->setText(String(processor->getVoltage(1)), dontSendNotification);
            repetitionsEditLabel->setText(String(processor->getRepetitions(1)), dontSendNotification);
            interpulseEditLabel->setText(String(processor->getInterPulseInt(1)), dontSendNotification);



            if (processor->getIsBiphasic(1) == true) {
                if (biphasicButton->getToggleState()==false)
                    biphasicButton->triggerClick();
            } else {
                if (biphasicButton->getToggleState()==true)
                    biphasicButton->triggerClick();
            }

            if (processor->getNegFirst(1) == true) {
                if (negFirstButton->getToggleState()==false)
                    negFirstButton->triggerClick();
            } else {
                if (negFirstButton->getToggleState()==true)
                    negFirstButton->triggerClick();
            }


        }
        else
            if (chan1Button->getToggleState()==false && chan2Button->getToggleState() == false
                    && chan3Button->getToggleState()==false && chan4Button->getToggleState() == false)
                chan2Button->triggerClick();

    }
    else if (button == chan3Button)
    {
        if (button->getToggleState()==true){
            if (chan2Button->getToggleState()==true)
                chan2Button->triggerClick();
            if (chan1Button->getToggleState()==true)
                chan1Button->triggerClick();
            if (chan4Button->getToggleState()==true)
                chan4Button->triggerClick();

            processor->setChan(2);


            phaseEditLabel->setText(String(processor->getPhaseDuration(2)), dontSendNotification);
            interphaseEditLabel->setText(String(processor->getInterPhaseInt(2)), dontSendNotification);
            voltageEditLabel->setText(String(processor->getVoltage(2)), dontSendNotification);
            repetitionsEditLabel->setText(String(processor->getRepetitions(2)), dontSendNotification);
            interpulseEditLabel->setText(String(processor->getInterPulseInt(2)), dontSendNotification);



            if (processor->getIsBiphasic(2) == true) {
                if (biphasicButton->getToggleState()==false)
                    biphasicButton->triggerClick();
            } else {
                if (biphasicButton->getToggleState()==true)
                    biphasicButton->triggerClick();
            }

            if (processor->getNegFirst(2) == true) {
                if (negFirstButton->getToggleState()==false)
                    negFirstButton->triggerClick();
            } else {
                if (negFirstButton->getToggleState()==true)
                    negFirstButton->triggerClick();
            }
        }
        else
            if (chan1Button->getToggleState()==false && chan2Button->getToggleState() == false
                    && chan3Button->getToggleState()==false && chan4Button->getToggleState() == false)
                chan3Button->triggerClick();

    }
    else if (button == chan4Button)
    {
        if (button->getToggleState()==true){
            if (chan2Button->getToggleState()==true)
                chan2Button->triggerClick();
            if (chan3Button->getToggleState()==true)
                chan3Button->triggerClick();
            if (chan1Button->getToggleState()==true)
                chan1Button->triggerClick();

            processor->setChan(3);


            phaseEditLabel->setText(String(processor->getPhaseDuration(3)), dontSendNotification);
            interphaseEditLabel->setText(String(processor->getInterPhaseInt(3)), dontSendNotification);
            voltageEditLabel->setText(String(processor->getVoltage(3)), dontSendNotification);
            repetitionsEditLabel->setText(String(processor->getRepetitions(3)), dontSendNotification);
            interpulseEditLabel->setText(String(processor->getInterPulseInt(3)), dontSendNotification);



            if (processor->getIsBiphasic(3) == true) {
                if (biphasicButton->getToggleState()==false)
                    biphasicButton->triggerClick();
            } else {
                if (biphasicButton->getToggleState()==true)
                    biphasicButton->triggerClick();
            }

            if (processor->getNegFirst(3) == true) {
                if (negFirstButton->getToggleState()==false)
                    negFirstButton->triggerClick();
            } else {
                if (negFirstButton->getToggleState()==true)
                    negFirstButton->triggerClick();
            }
        }
        else
            if (chan1Button->getToggleState()==false && chan2Button->getToggleState() == false
                    && chan3Button->getToggleState()==false && chan4Button->getToggleState() == false)
                chan4Button->triggerClick();

    }

    processor->updatePulsePal();
    repaint();

}

bool PulsePalOutputCanvas::keyPressed(const KeyPress &key, Component *originatingComponent)
{

}

void PulsePalOutputCanvas::labelTextChanged(Label *label)
{
    if (label == phaseEditLabel)
    {
        // 100 - 3600*10e3 (3600 s)
        Value val = label->getTextValue();
        float value = float(my_round(float(val.getValue())*10) / 10); //only multiple of 100us
        if ((float(val.getValue())>=0 && int(val.getValue())<=3600*10e3))
        {
            processor->setPhaseDuration(processor->getChan(), value);
            label->setText(String(value), dontSendNotification);
            if (!processor->checkParameterConsistency(processor->getChan()))
            {
                CoreServices::sendStatusMessage("Inconsistent Values!");
                //if values are inconsestent here phase is  > interpulse or trainduration
                processor->setRepetitions(processor->getChan(), 1);
                repetitionsEditLabel->setText(String(1), dontSendNotification);
            }
            trainDurationEditLabel->setText(String(processor->getTrainDuration(processor->getChan())), dontSendNotification);
            interpulseEditLabel->setText(String(processor->getInterPulseInt(processor->getChan())), dontSendNotification);
        }
        else
        {
            CoreServices::sendStatusMessage("Selected values must be within 0 and 3600*10e3 with 0.1 step!");
            label->setText("", dontSendNotification);

        }
    }
    if (label == interphaseEditLabel)
    {
        Value val = label->getTextValue();
        float value = float(my_round(float(val.getValue())*10) / 10); //only multiple of 100us
        if ((float(val.getValue())>=0 && int(val.getValue())<=3600*10e3))
        {
            processor->setInterPhaseInt(processor->getChan(), value);
            label->setText(String(value), dontSendNotification);
            if (!processor->checkParameterConsistency(processor->getChan()))
            {
                CoreServices::sendStatusMessage("Inconsistent Values!");
                processor->setInterPhaseInt(processor->getChan(), DEF_INTER_PHASE);
                label->setText("", dontSendNotification);
            }
        }
        else
        {
            CoreServices::sendStatusMessage("Selected values must be within 0 and 3600*10e3 with 0.1 step!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == voltageEditLabel)
    {
        Value val = label->getTextValue();
        if (float(val.getValue())>=0 && float(val.getValue())<=10)
        {
            //            float value = float(int(float(val.getValue())*100) / 5 * 5)/100; //only multiple of 100us
            processor->setVoltage(processor->getChan(), float(val.getValue()));
        }
        else
        {
            CoreServices::sendStatusMessage("Selected values must be within 0 and 10 with 0.05 step!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == repetitionsEditLabel)
    {
        Value val = label->getTextValue();
        if (int(val.getValue())>=1)
        {
            processor->setRepetitions(processor->getChan(), int(val.getValue()));
            processor->setRepetitionsTrainDuration(processor->getChan(), processor->REPFIRST);
            trainDurationEditLabel->setText(String(processor->getTrainDuration(processor->getChan())), dontSendNotification);
            if (!processor->checkParameterConsistency(processor->getChan()))
            {
                CoreServices::sendStatusMessage("Inconsistent Values!");
                processor->setRepetitions(processor->getChan(), DEF_REPETITIONS);
                label->setText("", dontSendNotification);
            }
        }
        else
        {
            CoreServices::sendStatusMessage("Selected values cannot be less than 1!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == interpulseEditLabel)
    {
        Value val = label->getTextValue();
        float value = float(my_round(float(val.getValue())*10) / 10); //only multiple of 100us
        if ((float(val.getValue())>=0 && int(val.getValue())<=3600*10e3))
        {
            processor->setInterPulseInt(processor->getChan(), value);
            label->setText(String(value), dontSendNotification);
            processor->setRepetitionsTrainDuration(processor->getChan(), processor->REPFIRST);
            trainDurationEditLabel->setText(String(processor->getTrainDuration(processor->getChan())), dontSendNotification);
            if (!processor->checkParameterConsistency(processor->getChan()))
            {
                CoreServices::sendStatusMessage("Inconsistent Values!");
                processor->setInterPulseInt(processor->getChan(), DEF_INTER_PULSE);
                label->setText("", dontSendNotification);
            }
        }
        else
        {
            CoreServices::sendStatusMessage("Selected values must be within 0 and 3600*10e3 with 0.1 step!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == trainDurationEditLabel)
    {
        Value val = label->getTextValue();
        float value = float(my_round(float(val.getValue())*10) / 10); //only multiple of 100us
        if (int(val.getValue())>=1)
        {
            processor->setTrainDuration(processor->getChan(), value);

            processor->setRepetitionsTrainDuration(processor->getChan(), processor->TRAINFIRST);
            repetitionsEditLabel->setText(String(processor->getRepetitions(processor->getChan())), dontSendNotification);
            if (!processor->checkParameterConsistency(processor->getChan()))
            {
                CoreServices::sendStatusMessage("Inconsistent Values!");
                //processor->setTrainDuration(processor->getChan(), DEF_TRAINDURATION);
                label->setText("", dontSendNotification);
            }
            label->setText(String(processor->getTrainDuration(processor->getChan())), dontSendNotification);
        }
        else
        {
            CoreServices::sendStatusMessage("Selected values cannot be less than 1!");
            label->setText("", dontSendNotification);
        }
    }

    processor->updatePulsePal();
}

void PulsePalOutputCanvas::initButtons()
{


    biphasicButton = new UtilityButton("biphasic", Font("Small Text", 13, Font::plain));
    biphasicButton->setRadius(3.0f);
    biphasicButton->addListener(this);
    biphasicButton->setClickingTogglesState(true);
//    biphasicButton->triggerClick();
    addAndMakeVisible(biphasicButton);

    negFirstButton = new UtilityButton("neg", Font("Small Text", 13, Font::plain));
    negFirstButton->setRadius(3.0f);
    negFirstButton->addListener(this);
    negFirstButton->setClickingTogglesState(true);
//    negFirstButton->triggerClick();
    addAndMakeVisible(negFirstButton);

    posFirstButton = new UtilityButton("pos", Font("Small Text", 13, Font::plain));
    posFirstButton->setRadius(3.0f);
    posFirstButton->addListener(this);
    posFirstButton->setClickingTogglesState(true);
    addAndMakeVisible(posFirstButton);

    chan1Button = new UtilityButton("1", Font("Small Text", 13, Font::plain));
    chan1Button->setRadius(3.0f);
    chan1Button->addListener(this);
    chan1Button->setClickingTogglesState(true);
    chan1Button->triggerClick();
    addAndMakeVisible(chan1Button);

    chan2Button = new UtilityButton("2", Font("Small Text", 13, Font::plain));
    chan2Button->setRadius(3.0f);
    chan2Button->addListener(this);
    chan2Button->setClickingTogglesState(true);
    addAndMakeVisible(chan2Button);

    chan3Button = new UtilityButton("3", Font("Small Text", 13, Font::plain));
    chan3Button->setRadius(3.0f);
    chan3Button->addListener(this);
    chan3Button->setClickingTogglesState(true);
    addAndMakeVisible(chan3Button);

    chan4Button = new UtilityButton("4", Font("Small Text", 13, Font::plain));
    chan4Button->setRadius(3.0f);
    chan4Button->addListener(this);
    chan4Button->setClickingTogglesState(true);
    addAndMakeVisible(chan4Button);

    if (processor->getIsBiphasic(0))
        biphasicButton->triggerClick();
    if (processor->getNegFirst(0))
        negFirstButton->triggerClick();
    else
        posFirstButton->triggerClick();


}

void PulsePalOutputCanvas::initLabels()
{


    //remove all and move to pulse pal output
    pulsePalLabel = new Label("s_pulsePal", "Pulse Pal Status: ");
    pulsePalLabel->setFont(Font(20));
    pulsePalLabel->setColour(Label::textColourId, labelColour);
    addAndMakeVisible(pulsePalLabel);


    pulsePalLabel = new Label("s_pp", "Pulse Pal:");
    pulsePalLabel->setFont(Font(20));
    pulsePalLabel->setColour(Label::textColourId, labelColour);
    addAndMakeVisible(pulsePalLabel);

    phaseLabel = new Label("s_phase", "phase [ms]:");
    phaseLabel->setFont(Font(15));
    phaseLabel->setColour(Label::textColourId, labelColour);
    addAndMakeVisible(phaseLabel);

    interphaseLabel = new Label("s_interphase", "interphase [ms]:");
    interphaseLabel->setFont(Font(15));
    interphaseLabel->setColour(Label::textColourId, labelColour);
    addAndMakeVisible(interphaseLabel);

    voltageLabel = new Label("s_voltage", "voltage [V]:");
    voltageLabel->setFont(Font(15));
    voltageLabel->setColour(Label::textColourId, labelColour);
    addAndMakeVisible(voltageLabel);

    repetitionsLabel = new Label("s_repetitions", "repetitions:");
    repetitionsLabel->setFont(Font(15));
    repetitionsLabel->setColour(Label::textColourId, labelColour);
    addAndMakeVisible(repetitionsLabel);

    trainDurationLabel = new Label("s_trainDuration", "trainDuration [ms]:");
    trainDurationLabel->setFont(Font(15));
    trainDurationLabel->setColour(Label::textColourId, labelColour);
    addAndMakeVisible(trainDurationLabel);

    interpulseLabel = new Label("s_interpulse", "interpulse [ms]:");
    interpulseLabel->setFont(Font(15));
    interpulseLabel->setColour(Label::textColourId, labelColour);
    addAndMakeVisible(interpulseLabel);


    phaseEditLabel = new Label("phase", String(DEF_PHASE_DURATION));
    phaseEditLabel->setFont(Font(15));
    phaseEditLabel->setColour(Label::textColourId, labelTextColour);
    phaseEditLabel->setColour(Label::backgroundColourId, labelBackgroundColour);
    phaseEditLabel->setEditable(true);
    phaseEditLabel->addListener(this);
    addAndMakeVisible(phaseEditLabel);

    interphaseEditLabel = new Label("interphase", String(DEF_INTER_PHASE));
    interphaseEditLabel->setFont(Font(15));
    interphaseEditLabel->setColour(Label::textColourId, labelTextColour);
    interphaseEditLabel->setColour(Label::backgroundColourId, labelBackgroundColour);
    interphaseEditLabel->setEditable(true);
    interphaseEditLabel->addListener(this);
    addAndMakeVisible(interphaseEditLabel);

    voltageEditLabel = new Label("voltage", String(DEF_VOLTAGE));
    voltageEditLabel->setFont(Font(15));
    voltageEditLabel->setColour(Label::textColourId, labelTextColour);
    voltageEditLabel->setColour(Label::backgroundColourId, labelBackgroundColour);
    voltageEditLabel->setEditable(true);
    voltageEditLabel->addListener(this);
    addAndMakeVisible(voltageEditLabel);

    repetitionsEditLabel = new Label("repetitions", String(DEF_REPETITIONS));
    repetitionsEditLabel->setFont(Font(15));
    repetitionsEditLabel->setColour(Label::textColourId, labelTextColour);
    repetitionsEditLabel->setColour(Label::backgroundColourId, labelBackgroundColour);
    repetitionsEditLabel->setEditable(true);
    repetitionsEditLabel->addListener(this);
    addAndMakeVisible(repetitionsEditLabel);

    trainDurationEditLabel = new Label("trainDuration", String(DEF_TRAINDURATION));
    trainDurationEditLabel->setFont(Font(15));
    trainDurationEditLabel->setColour(Label::textColourId, labelTextColour);
    trainDurationEditLabel->setColour(Label::backgroundColourId, labelBackgroundColour);
    trainDurationEditLabel->setEditable(true);
    trainDurationEditLabel->addListener(this);
    addAndMakeVisible(trainDurationEditLabel);

    interpulseEditLabel = new Label("interpulse", String(DEF_INTER_PULSE));
    interpulseEditLabel->setFont(Font(15));
    interpulseEditLabel->setColour(Label::textColourId, labelTextColour);
    interpulseEditLabel->setColour(Label::backgroundColourId, labelBackgroundColour);
    interpulseEditLabel->setEditable(true);
    interpulseEditLabel->addListener(this);
    addAndMakeVisible(interpulseEditLabel);


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
