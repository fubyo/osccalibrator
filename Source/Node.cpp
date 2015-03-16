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


#include "Node.h"

Node::Node ()
    : title (0)
{
    addAndMakeVisible (title = new Label (T("title label"),
                                          T("Title")));
    title->setFont (Font (15.0000f, Font::plain));
    title->setJustificationType (Justification::centredLeft);
    title->setEditable (false, true, false);
    title->setColour (TextEditor::textColourId, Colours::black);
    title->setColour (TextEditor::backgroundColourId, Colour (0x0));

    setSize (100, 40);

	id = -1;
	selected=false;
//	processingPriority = numeric_limits<int>::max();
	processingPriority = 0;
	type = -1; //unspecified
}

Node::~Node()
{
    deleteAndZero (title);

	for (unsigned int i=0; i<outputConnectors.size(); i++)
		deleteAndZero(outputConnectors[i]);

	for (unsigned int i=0; i<inputConnectors.size(); i++)
		deleteAndZero(inputConnectors[i]);
}

//==============================================================================
void Node::paint (Graphics& g)
{
    g.fillAll (Colours::aliceblue);
}

void Node::resized()
{
    title->setBounds (0, 0, proportionOfWidth (1.0000f), 20);

	for (unsigned int i=0; i<inputConnectors.size(); i++)
	{
		inputConnectors[i]->setBounds(0, 20+i*20, 60, 15);
	}

	for (unsigned int i=0; i<outputConnectors.size(); i++)
	{
		outputConnectors[i]->setBounds(40, 20+inputConnectors.size()*20+i*20, 60, 15);
	}
}

void Node::mouseDown(const MouseEvent& e)
{
	if (e.mods.isLeftButtonDown())
	{
		dragger.startDraggingComponent(this, e);
	}
}

void Node::mouseDrag(const MouseEvent& e)
{
	if (e.mods.isLeftButtonDown())
	{
		dragger.dragComponent(this, e, nullptr);
	}
}

int Node::getID()
{
	return id;
}

void Node::setID(int Id)
{
	id=Id;
}

Point<int> Node::getInputConnectorLocalCoordinates(int Id)
{
	Point<int> result;

	if (Id>=0 && Id<(int)inputConnectors.size())
	{
		result.setX(0);
		result.setY(20+Id*20+7);
	}
	else
	{
		result.setXY(-1, -1);
	}
	
	return result;
}

Point<int> Node::getOutputConnectorLocalCoordinates(int Id)
{
	Point<int> result;

	if (Id>=0 && Id<(int)outputConnectors.size())
	{
		result.setX(100);
		result.setY(20+inputConnectors.size()*20+Id*20+7);
	}
	else
	{
		result.setXY(-1, -1);
	}
	
	return result;
}

void Node::setConnectivityOfInput(int ConnectorId, bool IsConnected)
{
	if (ConnectorId>=0 && ConnectorId<(int)inputConnectors.size())
	{
		inputConnectors[ConnectorId]->setConnected(IsConnected);
	}
}

void Node::setConnectivityOfOutput(int ConnectorId, bool IsConnected)
{
	if (ConnectorId>=0 && ConnectorId<(int)outputConnectors.size())
	{
		outputConnectors[ConnectorId]->setConnected(IsConnected);
	}
}

void Node::setSelected(bool Selected)
{
	selected=Selected;
	if (selected)
		title->setColour (Label::backgroundColourId, Colour (0xffe6a7a2));
	else
		title->setColour (Label::backgroundColourId, Colour (0x0));
}

bool Node::isSelected()
{
	return selected;
}

int Node::getNumberOfInputs()
{
	return (int)inputConnectors.size();
}

int Node::getNumberOfOutputs()
{
	return (int)outputConnectors.size();
}

bool Node::isAnInputConnected()
{
	for (int i=0; i<(int)inputConnectors.size(); i++)
	{
		if (inputConnectors[i]->isItConnected())
			return true;
	}
	return false;
}

bool Node::isAnOutputConnected()
{
	for (int i=0; i<(int)outputConnectors.size(); i++)
	{
		if (outputConnectors[i]->isItConnected())
			return true;
	}
	return false;
}

float Node::getInputValue(int ConnectorIndex)
{
	if (ConnectorIndex < (int)inputConnectors.size())
		return inputConnectors[ConnectorIndex]->getValue();
	else return 0;
}

float Node::getOutputValue(int ConnectorIndex)
{
	if (ConnectorIndex < (int)outputConnectors.size())
		return outputConnectors[ConnectorIndex]->getValue();
	else return 0;
}

void Node::setInputValue(int ConnectorIndex, float Value)
{
	if (ConnectorIndex < (int)inputConnectors.size())
		inputConnectors[ConnectorIndex]->setValue(Value);
}

void Node::setOutputValue(int ConnectorIndex, float Value)
{
	if (ConnectorIndex < (int)outputConnectors.size())
		outputConnectors[ConnectorIndex]->setValue(Value);
}

void Node::process()
{
}

int Node::getType()
{
	return type;
}

String Node::getTitle()
{
	return title->getText();
}

void Node::setTitle(String title)
{
	this->title->setText(title, true);
}