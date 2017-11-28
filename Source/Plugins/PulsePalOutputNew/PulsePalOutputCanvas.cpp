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


#include "TrackingStimulatorCanvas.h"
#include "TrackingStimulator.h"


TrackingStimulatorCanvas::TrackingStimulatorCanvas(TrackingStimulator* TrackingStimulator)
    : processor(TrackingStimulator)
    , m_x(0.5)
    , m_y(0.5)
    , m_width(1.0)
    , m_height(1.0)
    , m_updateCircle(true)
    , m_onoff(false)
    , m_isDeleting(true)
    , selectedSource(-1)
    , buttonTextColour(Colour(255,255,255))
    , labelColour(Colour(200, 255, 0))
    , labelTextColour(Colour(255, 200, 0))
    , labelBackgroundColour(Colour(100,100,100))
    , backgroundColour(Colours::black)
{
    // Setup buttons
    initButtons();
    // Setup Labels
    initLabels();

    addKeyListener(this);
    m_ax = new DisplayAxes(TrackingStimulator, this);

    startCallbacks();
    update();
}

TrackingStimulatorCanvas::~TrackingStimulatorCanvas()
{
    TopLevelWindow::getTopLevelWindow(0)->removeKeyListener(this);
}

float TrackingStimulatorCanvas::my_round(float x)
{
    return x < 0.0 ? ceil(x - 0.5) : floor(x + 0.5);
}

void TrackingStimulatorCanvas::paint (Graphics& g)
{
    if(m_x != m_x || m_y != m_y || m_width != m_width || m_height != m_height)
    { // is it nan?
        m_x = m_prevx;
        m_y = m_prevy;
    }

//    // set aspect ratio to cam size
//    float aC = m_width / m_height;
//    float aS = plot_width / plot_height;
//    int camHeight = (aS > aC) ? plot_height : plot_height * (aS / aC);
//    int camWidth = (aS < aC) ? plot_width : plot_width * (aC / aS);

    g.setColour(backgroundColour); // backbackround color
    g.fillRect(0, 0, getWidth(), getHeight());

    // on-off LED
    if (m_onoff)
        g.setColour(labelColour);
    else
        g.setColour(labelBackgroundColour);
    g.fillEllipse(getWidth() - 0.065*getWidth(), 0.16*getHeight(), 0.03*getWidth(), 0.03*getHeight());

    // Check pulse Pal connection
    if (processor->getPulsePalVersion() > 0)
        pulsePalLabel->setText(String("Pulse Pal: ") +=  String("CONNECTED"), dontSendNotification);
    else
        pulsePalLabel->setText(String("Pulse Pal: ") +=  String("NOT CONNECTED"), dontSendNotification);

    m_ax->repaint();

}

void TrackingStimulatorCanvas::resized()
{
    m_ax->setBounds(int(0.01*getHeight()), int(0.01*getHeight()), int(0.98*getHeight()), int(0.98*getHeight()));
    addAndMakeVisible(m_ax);

    // Express all positions as proportion of height and width
    // Buttons

    saveButton->setBounds(getWidth() - 0.35*getWidth(), getHeight()-0.3*getHeight(), 0.1*getWidth(),0.05*getHeight());
    saveAsButton->setBounds(getWidth() - 0.35*getWidth(), getHeight()-0.25*getHeight(), 0.1*getWidth(),0.05*getHeight());
    loadButton->setBounds(getWidth() - 0.35*getWidth(), getHeight()-0.2*getHeight(), 0.1*getWidth(),0.05*getHeight());
    clearButton->setBounds(getWidth() - 0.35*getWidth(), getHeight()-0.1*getHeight(), 0.1*getWidth(),0.05*getHeight());

    simTrajectoryButton->setBounds(getWidth() - 0.35*getWidth(), getHeight()-0.4*getHeight(), 0.1*getWidth(),0.05*getHeight());

    newButton->setBounds(getWidth() - 0.2*getWidth(), 0.05*getHeight(), 0.06*getWidth(),0.04*getHeight());
    editButton->setBounds(getWidth() - 0.14*getWidth(), 0.05*getHeight(), 0.06*getWidth(),0.04*getHeight());
    delButton->setBounds(getWidth() - 0.08*getWidth(), 0.05*getHeight(), 0.06*getWidth(),0.04*getHeight());

    onButton->setBounds(getWidth() - 0.065*getWidth(), 0.21*getHeight(), 0.03*getWidth(),0.03*getHeight());

    for (int i = 0; i<MAX_CIRCLES; i++)
    {
        circlesButton[i]->setBounds(getWidth() - 0.2*getWidth()+i*(0.18/MAX_CIRCLES)*getWidth(), 0.25*getHeight(),
                                    (0.18/MAX_CIRCLES)*getWidth(),0.03*getHeight());
        if (i<processor->getCircles().size())
            circlesButton[i]->setVisible(true);
        else
            circlesButton[i]->setVisible(false);
    }

    chan1Button->setBounds(getWidth() - 0.2*getWidth(), 0.32*getHeight(), 0.045*getWidth(),0.05*getHeight());
    chan2Button->setBounds(getWidth() - 0.2*getWidth() + 0.045*getWidth(), 0.32*getHeight(), 0.045*getWidth(),0.05*getHeight());
    chan3Button->setBounds(getWidth() - 0.2*getWidth() + 0.09 *getWidth(), 0.32*getHeight(), 0.045*getWidth(),0.05*getHeight());
    chan4Button->setBounds(getWidth() - 0.2*getWidth() + 0.135*getWidth(), 0.32*getHeight(), 0.045*getWidth(),0.05*getHeight());

    uniformButton->setBounds(getWidth() - 0.2*getWidth(), 0.55*getHeight(), 0.09*getWidth(),0.03*getHeight());
    gaussianButton->setBounds(getWidth() - 0.2*getWidth() + 0.09*getWidth(), 0.55*getHeight(), 0.09*getWidth(),0.03*getHeight());

    biphasicButton->setBounds(getWidth() - 0.2*getWidth(), 0.6*getHeight(), 0.18*getWidth(),0.03*getHeight());
    negFirstButton->setBounds(getWidth() - 0.2*getWidth(), 0.65*getHeight(), 0.09*getWidth(),0.03*getHeight());
    posFirstButton->setBounds(getWidth() - 0.2*getWidth() + 0.09*getWidth(), 0.65*getHeight(), 0.09*getWidth(),0.03*getHeight());

    availableChans->setBounds(getWidth() - 0.4*getWidth(), 0.1*getHeight(), 0.18*getWidth(),0.04*getHeight());


//    // Static Labels
//    pulsePalLabel->setBounds(getWidth() - 0.4*getWidth(), 0.1*getHeight(), 0.18*getWidth(),0.04*getHeight());

//    cxLabel->setBounds(getWidth() - 0.2*getWidth(), 0.1*getHeight(), 0.06*getWidth(),0.04*getHeight());
//    cyLabel->setBounds(getWidth() - 0.2*getWidth(), 0.15*getHeight(), 0.06*getWidth(),0.04*getHeight());
//    cradLabel->setBounds(getWidth() - 0.2*getWidth(), 0.2*getHeight(), 0.06*getWidth(),0.04*getHeight());

//    fmaxLabel->setBounds(getWidth() - 0.2*getWidth(), 0.39*getHeight(), 0.1*getWidth(),0.04*getHeight());
//    sdevLabel->setBounds(getWidth() - 0.2*getWidth(), 0.44*getHeight(), 0.1*getWidth(),0.04*getHeight());
////    elecLabel->setBounds(getWidth() - 0.2*getWidth(), 0.49*getHeight(), 0.1*getWidth(),0.04*getHeight());

//    phaseLabel->setBounds(getWidth() - 0.2*getWidth(), 0.7*getHeight(), 0.1*getWidth(),0.04*getHeight());
//    interphaseLabel->setBounds(getWidth() - 0.2*getWidth(), 0.75*getHeight(), 0.1*getWidth(),0.04*getHeight());
//    voltageLabel->setBounds(getWidth() - 0.2*getWidth(), 0.8*getHeight(), 0.1*getWidth(),0.04*getHeight());
//    interpulseLabel->setBounds(getWidth() - 0.2*getWidth(), 0.85*getHeight(), 0.1*getWidth(),0.04*getHeight());
//    repetitionsLabel->setBounds(getWidth() - 0.2*getWidth(), 0.9*getHeight(), 0.1*getWidth(),0.04*getHeight());
//    trainDurationLabel->setBounds(getWidth() - 0.2*getWidth(), 0.95*getHeight(), 0.1*getWidth(),0.04*getHeight());


//    // Edit Labels
//    cxEditLabel->setBounds(getWidth() - 0.14*getWidth(), 0.1*getHeight(), 0.06*getWidth(),0.04*getHeight());
//    cyEditLabel->setBounds(getWidth() - 0.14*getWidth(), 0.15*getHeight(), 0.06*getWidth(),0.04*getHeight());
//    cradEditLabel->setBounds(getWidth() - 0.14*getWidth(), 0.2*getHeight(), 0.06*getWidth(),0.04*getHeight());

//    fmaxEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.39*getHeight(), 0.08*getWidth(),0.04*getHeight());
//    sdevEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.44*getHeight(), 0.08*getWidth(),0.04*getHeight());
////    elecEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.49*getHeight(), 0.08*getWidth(),0.04*getHeight());

//    phaseEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.7*getHeight(), 0.08*getWidth(),0.04*getHeight());
//    interphaseEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.75*getHeight(), 0.08*getWidth(),0.04*getHeight());
//    voltageEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.8*getHeight(), 0.08*getWidth(),0.04*getHeight());
//    interpulseEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.85*getHeight(), 0.08*getWidth(),0.04*getHeight());
//    repetitionsEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.9*getHeight(), 0.08*getWidth(),0.04*getHeight());
//    trainDurationEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.95*getHeight(), 0.08*getWidth(),0.04*getHeight());

    refresh();
}

void TrackingStimulatorCanvas::buttonClicked(Button* button)
{
    if (button == clearButton)
    {
        clear();
    }
    else if (button == saveButton)
    {
        processor->save();
    }
    else if (button == saveAsButton)
    {
        processor->saveAs();
    }
    else if (button == loadButton)
    {
        processor->load();

        // update labels and buttons
        uploadInfoOnLoad();
    }
    else if (button == simTrajectoryButton)
    {
        if (simTrajectoryButton->getToggleState() == true)
            processor->setSimulateTrajectory(true);
        else
            processor->setSimulateTrajectory(false);
    }
    else if (button == newButton)
    {
        m_updateCircle = true;

        // Create new circle
        Value x = cxEditLabel->getTextValue();
        Value y = cyEditLabel->getTextValue();
        Value rad = cradEditLabel->getTextValue();
        if (processor->getCircles().size() < MAX_CIRCLES)
        {
            processor->addCircle(Circle(float(x.getValue()), float(y.getValue()), float(rad.getValue()), m_onoff));
            processor->setSelectedCircle(processor->getCircles().size()-1);
            circlesButton[processor->getSelectedCircle()]->setVisible(true);

            // toggle current circle button (untoggles all the others)
            if (circlesButton[processor->getSelectedCircle()]->getToggleState()==false)
                circlesButton[processor->getSelectedCircle()]->triggerClick();
            m_isDeleting = false;
        }
        else
            CoreServices::sendStatusMessage("Max number of circles reached!");


    }
    else if (button == editButton)
    {
        m_updateCircle = true;

        Value x = cxEditLabel->getTextValue();
        Value y = cyEditLabel->getTextValue();
        Value rad = cradEditLabel->getTextValue();
        if (areThereCicles())
            processor->editCircle(processor->getSelectedCircle(),x.getValue(),y.getValue(),rad.getValue(),m_onoff);
    }
    else if (button == delButton)
    {
        m_updateCircle = true;

        processor->deleteCircle(processor->getSelectedCircle());
        // make visible only the remaining labels
        for (int i = 0; i<MAX_CIRCLES; i++)
        {
            if (i<processor->getCircles().size())
                circlesButton[i]->setVisible(true);
            else
                circlesButton[i]->setVisible(false);
        }

        // Blank labels and untoggle all circle buttons
        // blank labels
        processor->setSelectedCircle(-1);
        cxEditLabel->setText(String(""), dontSendNotification);
        cyEditLabel->setText(String(""), dontSendNotification);
        cradEditLabel->setText(String(""), dontSendNotification);
        m_onoff = false;

        for (int i = 0; i<MAX_CIRCLES; i++)
//            circlesButton[i]->setEnabledState(false);
                circlesButton[i]->setToggleState(false, true);

    }
    else if (button == onButton)
    {
        m_onoff = !m_onoff;
        // make changes immediately if circle already exist
        if (processor->getSelectedCircle() != -1)
            editButton->triggerClick();
    }
    else if (button == uniformButton)
    {
        if (button->getToggleState()==true){
            if (gaussianButton->getToggleState()==true)
            {
                gaussianButton->triggerClick();
                sdevLabel->setVisible(false);
                sdevEditLabel->setVisible(false);
            }

            processor->setIsUniform(processor->getChan(), true);
        }
        else
            if (gaussianButton->getToggleState()==false)
            {
                gaussianButton->triggerClick();
                sdevLabel->setVisible(true);
                sdevEditLabel->setVisible(true);
            }
    }
    else if (button == gaussianButton)
    {
        if (button->getToggleState()==true){
            if (uniformButton->getToggleState()==true)
            {
                uniformButton->triggerClick();
                sdevLabel->setVisible(true);
                sdevEditLabel->setVisible(true);
            }

            processor->setIsUniform(processor->getChan(), false);
        }
        else
            if (uniformButton->getToggleState()==false)
            {
                uniformButton->triggerClick();
                sdevLabel->setVisible(false);
                sdevEditLabel->setVisible(false);
            }

    }
    else if (button == biphasicButton)
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
        uploadInfoOnLoad();
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
            fmaxEditLabel->setText(String(processor->getStimFreq(0)), dontSendNotification);
            sdevEditLabel->setText(String(processor->getStimSD(0)), dontSendNotification);
//            elecEditLabel->setText(String(processor->getStimElectrode(0)), dontSendNotification);
            phaseEditLabel->setText(String(processor->getPhaseDuration(0)), dontSendNotification);
            interphaseEditLabel->setText(String(processor->getInterPhaseInt(0)), dontSendNotification);
            voltageEditLabel->setText(String(processor->getVoltage(0)), dontSendNotification);
            repetitionsEditLabel->setText(String(processor->getRepetitions(0)), dontSendNotification);
            interpulseEditLabel->setText(String(processor->getInterPulseInt(0)), dontSendNotification);

            // toggle buttons
            if (processor->getIsUniform(0) == true){
                if (uniformButton->getToggleState()==false)
                    uniformButton->triggerClick();
            } else {
                if (uniformButton->getToggleState()==true)
                    uniformButton->triggerClick();
            }

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
            fmaxEditLabel->setText(String(processor->getStimFreq(1)), dontSendNotification);
            sdevEditLabel->setText(String(processor->getStimSD(1)), dontSendNotification);
//            elecEditLabel->setText(String(processor->getStimElectrode(1)), dontSendNotification);
            phaseEditLabel->setText(String(processor->getPhaseDuration(1)), dontSendNotification);
            interphaseEditLabel->setText(String(processor->getInterPhaseInt(1)), dontSendNotification);
            voltageEditLabel->setText(String(processor->getVoltage(1)), dontSendNotification);
            repetitionsEditLabel->setText(String(processor->getRepetitions(1)), dontSendNotification);
            interpulseEditLabel->setText(String(processor->getInterPulseInt(1)), dontSendNotification);

            // toggle buttons
            if (processor->getIsUniform(1) == true){
                if (uniformButton->getToggleState()==false)
                    uniformButton->triggerClick();
            } else {
                if (uniformButton->getToggleState()==true)
                    uniformButton->triggerClick();
            }

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

            // retrieve and set label values
            fmaxEditLabel->setText(String(processor->getStimFreq(2)), dontSendNotification);
            sdevEditLabel->setText(String(processor->getStimSD(2)), dontSendNotification);
//            elecEditLabel->setText(String(processor->getStimElectrode(2)), dontSendNotification);
            phaseEditLabel->setText(String(processor->getPhaseDuration(2)), dontSendNotification);
            interphaseEditLabel->setText(String(processor->getInterPhaseInt(2)), dontSendNotification);
            voltageEditLabel->setText(String(processor->getVoltage(2)), dontSendNotification);
            repetitionsEditLabel->setText(String(processor->getRepetitions(2)), dontSendNotification);
            interpulseEditLabel->setText(String(processor->getInterPulseInt(2)), dontSendNotification);

            // toggle buttons
            if (processor->getIsUniform(2) == true){
                if (uniformButton->getToggleState()==false)
                    uniformButton->triggerClick();
            } else {
                if (uniformButton->getToggleState()==true)
                    uniformButton->triggerClick();
            }

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

            // retrieve and set label values
            fmaxEditLabel->setText(String(processor->getStimFreq(3)), dontSendNotification);
            sdevEditLabel->setText(String(processor->getStimSD(3)), dontSendNotification);
//            elecEditLabel->setText(String(processor->getStimElectrode(3)), dontSendNotification);
            phaseEditLabel->setText(String(processor->getPhaseDuration(3)), dontSendNotification);
            interphaseEditLabel->setText(String(processor->getInterPhaseInt(3)), dontSendNotification);
            voltageEditLabel->setText(String(processor->getVoltage(3)), dontSendNotification);
            repetitionsEditLabel->setText(String(processor->getRepetitions(3)), dontSendNotification);
            interpulseEditLabel->setText(String(processor->getInterPulseInt(3)), dontSendNotification);

            // toggle buttons
            if (processor->getIsUniform(3) == true){
                if (uniformButton->getToggleState()==false)
                    uniformButton->triggerClick();
            } else {
                if (uniformButton->getToggleState()==true)
                    uniformButton->triggerClick();
            }

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
    else
    {
        // check if one of circle button has been clicked
        bool someToggled = false;
        for (int i = 0; i<MAX_CIRCLES; i++)
        {
            if (button == circlesButton[i] && circlesButton[i]->isVisible() )
            {
                // toggle button and untoggle all the others + update
                if (button->getToggleState()==true)
                {
                    for (int j = 0; j<MAX_CIRCLES; j++)
                        if (i!=j && circlesButton[j]->getToggleState()==true)
                        {
                            //                        circlesButton[j]->triggerClick();
//                            circlesButton[j]->setEnabledState(false);
                            circlesButton[j]->setToggleState(false, true);

                        }
                    processor->setSelectedCircle(i);
                    someToggled = true;
                    // retrieve labels and on button values
                    if (areThereCicles())
                    {
                        cxEditLabel->setText(String(processor->getCircles()[processor->getSelectedCircle()].getX()), dontSendNotification);
                        cyEditLabel->setText(String(processor->getCircles()[processor->getSelectedCircle()].getY()), dontSendNotification);
                        cradEditLabel->setText(String(processor->getCircles()[processor->getSelectedCircle()].getRad()), dontSendNotification);
                        m_onoff = processor->getCircles()[processor->getSelectedCircle()].getOn();
                    }
                }


            }
        }
        if (!someToggled)
        {
            // blank labels
            processor->setSelectedCircle(-1);
            cxEditLabel->setText(String(""), dontSendNotification);
            cyEditLabel->setText(String(""), dontSendNotification);
            cradEditLabel->setText(String(""), dontSendNotification);
            m_onoff = false;

        }
    }
    processor->updatePulsePal();
    repaint();

}

void TrackingStimulatorCanvas::labelTextChanged(Label *label)
{
    // Instance a new circle only when the add new button is clicked
    if (label == cxEditLabel)
    {
        Value val = label->getTextValue();
        if (!(float(val.getValue())>=0 && float(val.getValue())<=1))
        {
            CoreServices::sendStatusMessage("Select value must be within 0 and 1!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == cyEditLabel)
    {
        Value val = label->getTextValue();
        if (!(float(val.getValue())>=0 && (float(val.getValue())<=1)))
        {
            CoreServices::sendStatusMessage("Select value must be within 0 and 1!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == cradEditLabel)
    {
        Value val = label->getTextValue();
        if (!(float(val.getValue())>=0 && (float(val.getValue())<=1)))
        {
            CoreServices::sendStatusMessage("Select value must be within 0 and 1!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == fmaxEditLabel)
    {
        Value val = label->getTextValue();
        if ((float(val.getValue())>=0 && float(val.getValue())<=10000))
            processor->setStimFreq(processor->getChan(), float(val.getValue()));
        else
        {
            CoreServices::sendStatusMessage("Selected values cannot be negative!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == sdevEditLabel)
    {
        Value val = label->getTextValue();
        if ((float(val.getValue())>=0 && float(val.getValue())<=1))
            if (float(val.getValue())>0)
                processor->setStimSD(processor->getChan(), float(val.getValue()));
            else
                processor->setStimSD(processor->getChan(), 1e-10);
        else
        {
            CoreServices::sendStatusMessage("Selected values must be between 0 and 1!");
            label->setText("", dontSendNotification);
        }
    }
//    if (label == elecEditLabel)
//    {
//        Value val = label->getTextValue();
//        if (int(val.getValue())>-1)
//            processor->setStimElectrode(processor->getChan(), int(val.getValue()));
//        else
//        {
//            CoreServices::sendStatusMessage("Selected values cannot be negative!");
//            label->setText("", dontSendNotification);
//        }
//    }
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


void TrackingStimulatorCanvas::refreshState()
{
}

void TrackingStimulatorCanvas::update()
{
    availableChans->clear();
    int nSources = processor->getNSources();
    int nextItem = 2;
    availableChans->addItem("None", 1);
    for (int i = 0; i < nSources; i++)
    {
        TrackingSources& source = processor->getTrackingSource(i);
        String name = String(source.sourceId) + " " + source.color;
        availableChans->addItem(name, nextItem++);
    }

}
