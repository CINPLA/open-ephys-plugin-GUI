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
#include "../../AccessClass.h"
#include "../../UI/EditorViewport.h"

SyncEditor::SyncEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors=true)
    : GenericEditor(parentNode, useDefaultParameterEditors)
      , syncChan(0)
{
    desiredWidth = 180;

    //sync
    syncButton = new UtilityButton("Sync", Font("Small Text", 13, Font::plain));
    syncButton->setRadius(3.0f); // sets the radius of the button's corners
    syncButton->setBounds(30, 50, 70, 40); // sets the x position, y position, width, and height of the button
    syncButton->addListener(this); // allows the editor to respond to clicks
    syncButton->setClickingTogglesState(false); // makes the button toggle its state when clicked
    addAndMakeVisible(syncButton); // makes the button a child component of the editor and makes it visible

    syncChanSelector = new ComboBox();
    syncChanSelector->setBounds(110, 70, 50, 20);
    syncChanSelector->addListener(this);

    for (int i=1; i<9; i++)
        syncChanSelector->addItem(String(i), i);

    syncChanSelector->setSelectedId(1, dontSendNotification);
    addAndMakeVisible(syncChanSelector);
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
            processor->syncEvent(syncChan);
        }
}

void SyncEditor::comboBoxChanged(ComboBox* c)
{
    syncChan = c->getSelectedId()-1;
}
