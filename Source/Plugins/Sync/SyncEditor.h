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

#ifndef SYNCEDITOR_H
#define SINCEDITOR_H


#include <EditorHeaders.h>

class SyncEditor : public GenericEditor,
        public ComboBox::Listener
{
public:
    SyncEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);
    virtual ~SyncEditor();

    void buttonEvent(Button* button);
    void comboBoxChanged(ComboBox* c);

private:
    // Listener Interface
    // Sync button
    ScopedPointer<UtilityButton> syncButton;
    ScopedPointer<ComboBox> syncChanSelector;
    int syncChan;

};


#endif  // SYNCEDITOR_H
