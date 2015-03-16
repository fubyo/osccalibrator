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
#include "OutputConnector.h"
#include "InputConnector.h"

#include <vector>
#include <limits>
using namespace std;

#define INPUTNODE 0
#define CALIBRATORNODE 1
#define OUTPUTNODE 2

class Node  : public Component
{
public:
    //==============================================================================
    Node ();
    ~Node();

    void paint (Graphics& g);
    void resized();

	void mouseDrag(const MouseEvent& e);
	void mouseDown(const MouseEvent& e);

	int getID();
	void setID(int Id);

	Point<int> getInputConnectorLocalCoordinates(int Id);
	Point<int> getOutputConnectorLocalCoordinates(int Id);

	void setConnectivityOfInput(int ConnectorId, bool IsConnected);
	void setConnectivityOfOutput(int ConnectorId, bool IsConnected);

	void setSelected(bool Selected);
	bool isSelected();

	int getNumberOfInputs();
	int getNumberOfOutputs();

	bool isAnInputConnected();
	bool isAnOutputConnected();

	float getInputValue(int ConnectorIndex);
	float getOutputValue(int ConnectorIndex);
	void setInputValue(int ConnectorIndex, float Value);
	void setOutputValue(int ConnectorIndex, float Value);

	int processingPriority;

	int getType();
	String getTitle();
	void setTitle(String title);
	OutputConnector* getOutputConnector(int index) {return outputConnectors[index];};
	InputConnector* getInputConnector(int index) {return inputConnectors[index];};
	void pushOutputConnector(OutputConnector* outputConnector) {outputConnectors.push_back(outputConnector);};
	void pushInputConnector(InputConnector* inputConnector) {inputConnectors.push_back(inputConnector);};

	virtual void process();
	

    //==============================================================================
    juce_UseDebuggingNewOperator

protected:
    //==============================================================================
    Label* title;
	vector<OutputConnector*> outputConnectors;
	vector<InputConnector*> inputConnectors;
	int id;
	bool selected;
	int type;

private:
	ComponentDragger dragger;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    Node (const Node&);
    const Node& operator= (const Node&);
};
