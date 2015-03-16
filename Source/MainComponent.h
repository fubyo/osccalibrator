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
#include "OscInputNode.h"
#include "OscOutputNode.h"
#include "CalibratorInitializer.h"
#include "CalibratorNode.h"
#include "CalibratorConfigurator.h"
#include "OscManager.h"
#include "AboutComponent.h"

#include <vector>
#include <limits>
#include <algorithm>
using namespace std;

struct Connection
{
	int outNodeId;
	int outConnectorId;
	int inNodeId;
	int inConnectorId;
};

class MainComponent : public Component, public KeyListener
{
public:
	MainComponent(void);
	~MainComponent(void);

	void paint (Graphics& g);
    void resized();

	void mouseUp (const MouseEvent &e);
	void mouseDrag (const MouseEvent &e);
	void mouseDown (const MouseEvent &e);

	bool keyPressed (const KeyPress & key, Component* originatingComponent); 

	bool connect(int outNodeId, int outConnectorId, int inNodeId, int inConnectorId);
	void disconnectInput(int NodeId, int InputId);
	void disconnectOutput(int NodeId, int OutputId);
	void deleteNode(Node* node);
	void deleteConnectionsOfNode(Node* node);

	void process();
	
	OscManager oscManager;
	CalibratorConfigurator* activeConfigurator;

private:
	vector<Node*> nodes;
	vector<Connection> connections;
	int runningNodeID;
	
	void sortNodes();

	Node* getNode(int Id);
	static bool nodeSortPredicate(const Node* a, const Node* b);

	CriticalSection cs;

	void loadConfiguration();
	void saveConfiguration();
};

