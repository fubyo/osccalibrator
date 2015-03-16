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


#include "OutputConnector.h"
#include "MainComponent.h"

OutputConnector::OutputConnector ()
    : label (0)
{
    addAndMakeVisible (label = new Label (T("new label"),
                                          T("out")));
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

OutputConnector::~OutputConnector()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (label);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void OutputConnector::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffc7c7c7));

    if (isConnected || dragInside)
		g.setColour (Colours::lightgreen);
	else
		g.setColour (Colour(0xff8c8c8c));

    g.fillRect (proportionOfWidth (0.9000f), 0, proportionOfWidth (0.1000f), proportionOfHeight (1.0000f));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void OutputConnector::resized()
{
    label->setBounds (proportionOfWidth (0.0000f), 0, proportionOfWidth (0.9000f), proportionOfHeight (1.0000f));
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

int OutputConnector::getID()
{
	return id;
}

void OutputConnector::setID(int Id)
{
	id=Id;
}

void OutputConnector::mouseDrag (const MouseEvent &e)
{
	int NodeID = ((Node*)getParentComponent())->getID();

	if (e.mods.isLeftButtonDown())
	{
		startDragging(String("out")+" "+String(NodeID)+" "+String(id), this, Image::null, true);
	}
}

void OutputConnector::mouseUp (const MouseEvent &e)
{
	if (e.mods.isRightButtonDown())
	{
		PopupMenu m;
		m.addItem (1, "Disconnect");
        const int result = m.show();

		if (result==1)
		{
			MainComponent* mainComponent = (MainComponent*)getParentComponent()->getParentComponent();
			mainComponent->disconnectOutput( ((Node*)getParentComponent())->getID(), id);
		}
	}
}

bool OutputConnector::isInterestedInDragSource (const String &dragSourceDetails, Component *sourceComponent)
{
	if (sourceComponent!=getParentComponent())
	{
		if (dragSourceDetails.contains("in") ||
			dragSourceDetails.contains("calibratorremote") ||  
			(((Node*)getParentComponent())->getType()==0 && (dragSourceDetails.contains("addremote") || dragSourceDetails.contains("clearremote"))) )
			return true;
		else
			return false;
	}	
	else
		return false;
}

void OutputConnector::itemDragEnter (const String &dragSourceDetails, Component* sourceComponent, int x, int y)
{
	dragInside=true;
	repaint();
}

void OutputConnector::itemDragExit (const String &dragSourceDetails,  Component* sourceComponent)
{
	dragInside=false;
	repaint();
}

void OutputConnector::itemDropped (const String &dragSourceDetails, Component* sourceComponent, int x, int y)	
{
	//Parse the dragSourceDetails

	if (dragSourceDetails.contains("in"))
	{
		String part=dragSourceDetails.fromFirstOccurrenceOf("in ", false, false);
		String InNodeId = part.upToFirstOccurrenceOf(" ", false, false);
		String InConnectorId = part.fromFirstOccurrenceOf(" ", false, false);

		MainComponent* mainComponent = (MainComponent*)getParentComponent()->getParentComponent();
		mainComponent->connect(((Node*)getParentComponent())->getID(), id, InNodeId.getIntValue(), InConnectorId.getIntValue());

		getParentComponent()->repaint();
	}
	else if (dragSourceDetails.contains("calibratorremote"))
	{
		String part=dragSourceDetails.fromFirstOccurrenceOf("calibratorremote ", false, false);
		int parameterIndex = part.getIntValue();

		MainComponent* mainComponent = (MainComponent*)getParentComponent()->getParentComponent();
		
		if (mainComponent->activeConfigurator)
		{
			mainComponent->activeConfigurator->configureRemote(parameterIndex, true, (Node*)getParentComponent(), id);
			sourceComponent->repaint();

			getParentComponent()->repaint();
		}
	}
	else if (dragSourceDetails.contains("addremote"))
	{
		MainComponent* mainComponent = (MainComponent*)getParentComponent()->getParentComponent();
		
		if (mainComponent->activeConfigurator)
		{
			mainComponent->activeConfigurator->setRemoteAdding(true);

			String address = ((OscInputNode*)getParentComponent())->getAddress();
			int port = ((OscInputNode*)getParentComponent())->getPort();
			
			mainComponent->oscManager.setRemoteAdding(true, address, port); 
		}
	}
	else if (dragSourceDetails.contains("clearremote"))
	{
		MainComponent* mainComponent = (MainComponent*)getParentComponent()->getParentComponent();

		if (mainComponent->activeConfigurator)
		{
			mainComponent->activeConfigurator->setRemoteClearing(true);

			String address = ((OscInputNode*)getParentComponent())->getAddress();
			int port = ((OscInputNode*)getParentComponent())->getPort();
			
			mainComponent->oscManager.setRemoteClearing(true, address, port); 
		}
	}

	dragInside=false;
}

void OutputConnector::setConnected(bool IsConnected)
{
	isConnected=IsConnected;
	repaint();
}

