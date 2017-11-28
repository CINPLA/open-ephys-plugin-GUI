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

#include "TrackingVisualizerEditor.h"
#include "TrackingVisualizerCanvas.h"
#include "TrackingVisualizer.h"

TrackingVisualizerEditor::TrackingVisualizerEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors=true)
    : VisualizerEditor(parentNode, useDefaultParameterEditors)
{
    tabText = "Tracking";
    desiredWidth = 180;
}

TrackingVisualizerEditor::~TrackingVisualizerEditor()
{
   // deleteAllChildren();
}

Visualizer* TrackingVisualizerEditor::createNewCanvas()
{
    TrackingVisualizer* processor = (TrackingVisualizer*) getProcessor();
    return new TrackingVisualizerCanvas(processor);
}

//void TrackingVisualizerEditor::saveCustomParameters(XmlElement *parentElement)
//{

//}

//void TrackingVisualizerEditor::loadCustomParameters(XmlElement *parametersAsXml)
//{

//}

void TrackingVisualizerEditor::initializeButtons()
{
//    int w = 18;
//    int h = 18;
//    int xPad = 5;
//    int yPad = 6;

//    int xInitial = 10;
//    int yInitial = 25;
//    int x = xInitial;
//    int y = yInitial;

//    clearBtn = new UtilityButton("Clear", titleFont);
//    clearBtn->setBounds(x, y, w*2 + xPad, h);
//    clearBtn->setClickingTogglesState(false);
//    clearBtn->addListener(this);
//    addAndMakeVisible(clearBtn);

}

void TrackingVisualizerEditor::buttonCallback(Button* button)
{
//    int gId = button->getRadioGroupId();

//    if (gId > 0)
//    {
//        if (canvas != 0)
//        {
//            canvas->setParameter(gId-1, button->getName().getFloatValue());
//        }

//    }

}
