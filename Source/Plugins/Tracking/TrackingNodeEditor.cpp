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

#include "TrackingNodeEditor.h"
#include "TrackingNode.h"
#include "../../AccessClass.h"
#include "../../UI/EditorViewport.h"

TrackingNodeEditor::TrackingNodeEditor (GenericProcessor* parentNode, bool useDefaultParameterEditors = true)
    : GenericEditor (parentNode, useDefaultParameterEditors)

{
    desiredWidth = 180;

    TrackingNode* processor = (TrackingNode*)getProcessor();

    adrLabel = new Label ("Address", "Address:");
    adrLabel->setBounds (10, 60, 140, 25);
    addAndMakeVisible (adrLabel);
    DBG ("in editor set default address");
    String defaultAddress = "/red";
    labelAdr = new Label ("Address", defaultAddress);
    labelAdr->setBounds (80, 65, 80, 18);
    labelAdr->setFont (Font ("Default", 15, Font::plain));
    labelAdr->setColour (Label::textColourId, Colours::white);
    labelAdr->setColour (Label::backgroundColourId, Colours::grey);
    labelAdr->setEditable (true);
    labelAdr->addListener (this);
    addAndMakeVisible (labelAdr);
    processor->setAddress (defaultAddress);

    portLabel = new Label ("Port", "Port:");
    portLabel->setBounds (10, 30, 140, 25);
    addAndMakeVisible (portLabel);

    int defaultPort = 27020;
    labelPort = new Label ("Port", String (defaultPort));
    labelPort->setBounds (80, 35, 80, 18);
    labelPort->setFont (Font ("Default", 15, Font::plain));
    labelPort->setColour (Label::textColourId, Colours::white);
    labelPort->setColour (Label::backgroundColourId, Colours::grey);
    labelPort->setEditable (true);
    labelPort->addListener (this);
    addAndMakeVisible (labelPort);
    processor->setPort (defaultPort);

    colorLabel = new Label ("Color", "Color:");
    colorLabel->setBounds (10, 90, 140, 25);
    addAndMakeVisible (colorLabel);

    String defaultColor = "red";
    labelColor = new Label ("Color", String (defaultColor));
    labelColor->setBounds (80, 95, 80, 18);
    labelColor->setFont (Font ("Default", 15, Font::plain));
    labelColor->setColour (Label::textColourId, Colours::white);
    labelColor->setColour (Label::backgroundColourId, Colours::grey);
    labelColor->setEditable (true);
    labelColor->addListener (this);
    addAndMakeVisible (labelColor);
    processor->setColor (defaultColor);
}

TrackingNodeEditor::~TrackingNodeEditor()
{
    //deleteAllChildren();
}

void TrackingNodeEditor::labelTextChanged (Label* label)
{
    if (label == labelAdr)
    {
        Value val = label->getTextValue();

        TrackingNode* p = (TrackingNode*) getProcessor();
        p->setAddress (val.getValue());
    }

    if (label == labelPort)
    {
        Value val = label->getTextValue();

        TrackingNode* p = (TrackingNode*) getProcessor();
        p->setPort (val.getValue());
    }

    if (label == labelColor)
    {
        Value val = label->getTextValue();

        TrackingNode* p = (TrackingNode*) getProcessor();
        p->setColor (val.getValue());
    }
}



void TrackingNodeEditor::saveCustomParameters (XmlElement* parentElement)
{
    XmlElement* mainNode = parentElement->createNewChildElement ("TrackingNode");
    mainNode->setAttribute ("port", labelPort->getText());
    mainNode->setAttribute ("address", labelAdr->getText());
    mainNode->setAttribute ("color", labelColor->getText());
}

void TrackingNodeEditor::loadCustomParameters (XmlElement* parametersAsXml)
{
    if (parametersAsXml != nullptr)
    {
        forEachXmlChildElement (*parametersAsXml, mainNode)
        {
            if (mainNode->hasTagName ("TrackingNode"))
            {
                labelPort->setText (mainNode->getStringAttribute ("port"), sendNotification);
                labelAdr->setText (mainNode->getStringAttribute ("address"), sendNotification);
                labelColor->setText (mainNode->getStringAttribute ("color"), sendNotification);
            }
        }
    }
}
