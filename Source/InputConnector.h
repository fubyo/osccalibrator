/* OscCalibrator - A mapping and routing tool for use with the Open Sound Control protocol.
   Copyright (C) 2012  Dionysios Marinos - fewbio@googlemail.com

   This program is free software: you can redistribute it and/or modify it under the
   terms of the GNU General Public License as published by the Free Software Foundation,
   either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
   without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
   See the GNU General Public License for more details.
   You should have received a copy of the GNU General Public License along with this program.
   If not, see <http://www.gnu.org/licenses/>.
*/



#pragma once
#include "..\juce\juce_amalgamated.h"

class InputConnector  : public Component, public DragAndDropTarget, public DragAndDropContainer
{
public:
    //==============================================================================
    InputConnector ();
    ~InputConnector();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();

	int getID();
	void setID(int Id);

	void mouseDrag (const MouseEvent &e);
	void mouseUp (const MouseEvent &e);

	bool isInterestedInDragSource (const String &dragSourceDetails, Component *sourceComponent); 
	void itemDragEnter (const String &dragSourceDetails, Component* sourceComponent, int x, int y);
	void itemDragExit (const String &dragSourceDetails, Component* sourceComponent);
	void itemDropped (const String &dragSourceDetails, Component* sourceComponent, int x, int y);

	void setConnected(bool IsConnected);
	bool isItConnected() {return isConnected;};

	float getValue() {return value;};
	void setValue(float Value) {value=Value;};

	String getLabel() {return label->getText();};
	void setLabel(String text) {label->setText(text, true);};

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    Label* label;
	int id;
	bool isConnected;
	bool dragInside;
	float value;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    InputConnector (const InputConnector&);
    const InputConnector& operator= (const InputConnector&);
};

