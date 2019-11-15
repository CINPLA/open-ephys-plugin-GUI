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

#include "SyncEditor.h"
#include "SyncNode.h"

SyncEditor::SyncEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors=true)
    : GenericEditor(parentNode, useDefaultParameterEditors)
      , syncChan(0)
{
    desiredWidth = 180;

    SyncNode* processor = (SyncNode*) getProcessor();

    //sync
    syncButton = new UtilityButton("Sync", Font("Small Text", 13, Font::plain));
    syncButton->setRadius(3.0f); // sets the radius of the button's corners
    syncButton->setBounds(30, 30, 70, 40); // sets the x position, y position, width, and height of the button
    syncButton->addListener(this); // allows the editor to respond to clicks
    syncButton->setClickingTogglesState(false); // makes the button toggle its state when clicked
    addAndMakeVisible(syncButton); // makes the button a child component of the editor and makes it visible

    syncChanSelector = new ComboBox();
    syncChanSelector->setBounds(110, 40, 50, 20);
    syncChanSelector->addListener(this);

    for (int i=1; i<9; i++)
        syncChanSelector->addItem(String(i), i);

    syncChanSelector->setSelectedId(11, dontSendNotification);
    addAndMakeVisible(syncChanSelector);

    durationLabel = new Label ("dur", "Duration (ms):");
    durationLabel->setBounds (30, 80, 70, 40);
    addAndMakeVisible (durationLabel);

    durationEditLabel = new Label ("duration_edit", String(2));
    durationEditLabel->setBounds (110, 90, 50, 20);
    durationEditLabel->setFont (Font ("Default", 15, Font::plain));
    durationEditLabel->setColour (Label::textColourId, Colours::white);
    durationEditLabel->setColour (Label::backgroundColourId, Colours::grey);
    durationEditLabel->setEditable (true);
    durationEditLabel->addListener (this);
    addAndMakeVisible (durationEditLabel);
}

SyncEditor::~SyncEditor()
{
    // TODO should we delete all children, check JUCE docs
    // PS: Causes segfault if we do right now
//    deleteAllChildren();
}

void SyncEditor::buttonEvent(Button* button)
{
    if (button == syncButton)
        if (!button->isDown())
        {
            SyncNode *processor = (SyncNode *)getProcessor();
            processor->syncEvent();
        }
}

void SyncEditor::labelTextChanged (Label* labelThatHasChanged)
{
    Value val = labelThatHasChanged->getTextValue();
    int value = int(val.getValue()); //only multiple of 100us
    if (value>=0)
    {
        SyncNode* processor = (SyncNode*) getProcessor();
        processor->setTtlDuration(value);
        labelThatHasChanged->setText(String(value), dontSendNotification);
    }
    else
    {
        CoreServices::sendStatusMessage("Selected values must be greater or equal than 0!");
        labelThatHasChanged->setText("", dontSendNotification);
    }
}

void SyncEditor::comboBoxChanged(ComboBox* c)
{
    SyncNode* processor = (SyncNode*) getProcessor();
    syncChan = c->getSelectedId()-1;
    processor->setOutputChan(syncChan);

}

void SyncEditor::updateSettings()
{
    SyncNode* processor = (SyncNode*) getProcessor();
    durationEditLabel->setText(String(processor->getTtlDuration()), dontSendNotification);

    int outputChan = processor->getOutputChan() + 1;
    if (outputChan > syncChanSelector->getNumItems())
    {
        outputChan = syncChanSelector->getNumItems();
        processor->setOutputChan(outputChan - 1);
        syncChanSelector->setSelectedId(outputChan);
    }
    else
        syncChanSelector->setSelectedId(outputChan);

}
