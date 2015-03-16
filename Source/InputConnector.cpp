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


#include "InputConnector.h"
#include "MainComponent.h"

InputConnector::InputConnector ()
    : label (0)
{
    addAndMakeVisible (label = new Label (T("new label"),
                                          T("in")));
    label->setFont (Font (12.4000f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, true, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));
	label->addMouseListener(this, false);

    //[UserPreSize]
    //[/UserPreSize]

    setSize (60, 15);

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
	id=-1;
	isConnected=false;
	dragInside=false;
	value=0;
}

InputConnector::~InputConnector()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (label);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void InputConnector::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffc7c7c7));

	if (isConnected || dragInside)
		g.setColour (Colours::lightgreen);
	else
		g.setColour (Colour(0xff8c8c8c));

    g.fillRect (proportionOfWidth (0.0000f), 0, proportionOfWidth (0.1000f), proportionOfHeight (1.0000f));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void InputConnector::resized()
{
    label->setBounds (proportionOfWidth (0.1000f), 0, proportionOfWidth (0.9000f), proportionOfHeight (1.0000f));
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

int InputConnector::getID()
{
	return id;
}

void InputConnector::setID(int Id)
{
	id=Id;
}

void InputConnector::mouseDrag (const MouseEvent &e)
{
	int NodeID = ((Node*)getParentComponent())->getID();

	if (e.mods.isLeftButtonDown())
	{
		startDragging(String("in")+" "+String(NodeID)+" "+String(id), this, Image::null, true);
	}
}

void InputConnector::mouseUp (const MouseEvent &e)
{
	if (e.mods.isRightButtonDown())
	{
		PopupMenu m;
		m.addItem (1, "Disconnect");
        const int result = m.show();

		if (result==1)
		{
			MainComponent* mainComponent = (MainComponent*)getParentComponent()->getParentComponent();
			mainComponent->disconnectInput( ((Node*)getParentComponent())->getID(), id);
		}
	}
}

bool InputConnector::isInterestedInDragSource (const String &dragSourceDetails, Component *sourceComponent)
{
	if (sourceComponent!=getParentComponent())
	{
		if (dragSourceDetails.contains("out"))
			return true;
		else
			return false;
	}	
	else
		return false;
}

void InputConnector::itemDragEnter (const String &dragSourceDetails, Component* sourceComponent, int x, int y)
{
	dragInside=true;
	repaint();
}

void InputConnector::itemDragExit (const String &dragSourceDetails,  Component* sourceComponent)
{
	dragInside=false;
	repaint();
}

void InputConnector::itemDropped (const String &dragSourceDetails, Component* sourceComponent, int x, int y)	
{
	//Parse the dragSourceDetails
	String part=dragSourceDetails.fromFirstOccurrenceOf("out ", false, false);
	String OutNodeId = part.upToFirstOccurrenceOf(" ", false, false);
	String OutConnectorId = part.fromFirstOccurrenceOf(" ", false, false);

	MainComponent* mainComponent = (MainComponent*)getParentComponent()->getParentComponent();
	mainComponent->connect(OutNodeId.getIntValue(), OutConnectorId.getIntValue(), ((Node*)getParentComponent())->getID(), id);

	getParentComponent()->repaint();

	dragInside=false;
}

void InputConnector::setConnected(bool IsConnected)
{
	isConnected=IsConnected;
	repaint();
}


