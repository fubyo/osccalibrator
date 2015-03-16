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


#include "MainComponent.h"


MainComponent::MainComponent(void)
{
	setSize (800, 600);

	addKeyListener(this);
	runningNodeID = -1;

	oscManager.setMainComponent(this);
	oscManager.startThread();

	activeConfigurator = 0; 
}


MainComponent::~MainComponent(void)
{
	for (unsigned int i=0; i<nodes.size(); i++)
	{
		delete nodes[i];
	}

	oscManager.stop();
}

void MainComponent::paint (Graphics& g)
{
	g.fillAll (Colours::lightgrey);

	//Draw connections
	for (unsigned int i=0; i<connections.size(); i++)
	{
		Node* startNode=getNode(connections[i].outNodeId);
		Node* endNode=getNode(connections[i].inNodeId);

		Point<int> startPoint = startNode->getPosition()+startNode->getOutputConnectorLocalCoordinates(connections[i].outConnectorId);
		Point<int> endPoint = endNode->getPosition()+endNode->getInputConnectorLocalCoordinates(connections[i].inConnectorId);

		g.drawLine((float)startPoint.getX(), (float)startPoint.getY(), (float)endPoint.getX(), (float)endPoint.getY());
	}
}

void MainComponent::resized()
{

}

void MainComponent::mouseUp(const MouseEvent &e)
{
	if (e.mods == ModifierKeys::rightButtonModifier && e.eventComponent==this)
	{
		PopupMenu m;
        
		m.addItem (1, "Add OSC input...");
        m.addItem (2, "Add calibrator...");
		m.addItem (3, "Add OSC output...");

		m.addSeparator();
		
		m.addItem (4, "Load configuration...");
		m.addItem (5, "Save configuration...");

		m.addSeparator();

		m.addItem (6, "About...");

        const int result = m.show();

        if (result == 0)
        {
            // user dismissed the menu without picking anything
        }
        else if (result == 1)
        {
            // user picked item 1
			OscInputNode* node;
			node = new OscInputNode();
			runningNodeID++;
			node->setID(runningNodeID);
			node->setOscManager(&oscManager);
			node->addMouseListener(this, true);
			addAndMakeVisible(node);
			node->setTopLeftPosition(e.getPosition().getX(), e.getPosition().getY());
			nodes.push_back(node);
        }
        else if (result == 2)
        {
			Component* temp = new Component();
			temp->setBounds(e.getPosition().getX(), e.getPosition().getY(), 1, 1);
			this->addChildComponent(temp);

			CalibratorInitializer* calibratorInitializer = new CalibratorInitializer();
			int result = DialogWindow::showModalDialog("Calibrator set up...", calibratorInitializer, temp, Colours::lightgrey, false);

			if (result)
			{
				CalibratorNode* node = new CalibratorNode();
				node->initialise(calibratorInitializer->getNumberOfInputs(), calibratorInitializer->getNumberOfOutputs());
				runningNodeID++;
				node->setID(runningNodeID);
				node->addMouseListener(this, true);
				addAndMakeVisible(node);
				node->setTopLeftPosition(e.getPosition().getX(), e.getPosition().getY());
				nodes.push_back(node);
			}

			delete temp;
			delete calibratorInitializer;
        }
		else if (result == 3)
        {
            // user picked item 3
			OscOutputNode* node;
			node = new OscOutputNode();
			runningNodeID++;
			node->setID(runningNodeID);
			node->addMouseListener(this, true);
			addAndMakeVisible(node);
			node->setTopLeftPosition(e.getPosition().getX(), e.getPosition().getY());
			nodes.push_back(node);
        }
		else if (result == 4)
        {
			loadConfiguration();
        }
		else if (result == 5)
        {
			saveConfiguration();
        }
		else if (result == 6)
		{
			AboutComponent* aboutComponent = new AboutComponent();
			DialogWindow::showModalDialog("About...", aboutComponent, NULL, Colours::lightgrey, false);

			delete aboutComponent;
		}
	}
}

void MainComponent::mouseDrag(const MouseEvent &e)
{
	repaint();
}

bool MainComponent::connect(int outNodeId, int outConnectorId, int inNodeId, int inConnectorId)
{
	//Check for loops
	if (outNodeId == inNodeId)
		return false;
	else if (getNode(outNodeId)->processingPriority > getNode(inNodeId)->processingPriority && getNode(outNodeId)->processingPriority && getNode(inNodeId)->processingPriority)
		return false;

	//Check if inConnector already connected
	for (unsigned int i=0; i<connections.size(); i++)
		if (connections[i].inNodeId==inNodeId && connections[i].inConnectorId==inConnectorId)
			return false;
	
	Connection connection;
	connection.outNodeId=outNodeId;
	connection.outConnectorId=outConnectorId;
	connection.inNodeId=inNodeId;
	connection.inConnectorId=inConnectorId;

	connections.push_back(connection);

	getNode(outNodeId)->setConnectivityOfOutput(outConnectorId, true);
	getNode(inNodeId)->setConnectivityOfInput(inConnectorId, true);

	sortNodes();
	repaint();

	return true;
}

Node* MainComponent::getNode(int Id)
{
	Node* result=0;

	for (unsigned int i=0; i<nodes.size(); i++)
		if (nodes[i]->getID()==Id)
			result=nodes[i];

	return result;
}

void MainComponent::disconnectInput(int NodeId, int InputId)
{
	unsigned int i=0;
	int tempOutNode=-1;
	int tempOutConnector=-1;

	while (i<connections.size())
	{
		if (connections[i].inNodeId==NodeId && connections[i].inConnectorId==InputId)
		{
			getNode(connections[i].inNodeId)->setConnectivityOfInput(connections[i].inConnectorId, false);

			tempOutNode=connections[i].outNodeId;
			tempOutConnector=connections[i].outConnectorId;

			connections.erase(connections.begin()+i);
		}

		i++;
	}


	if (tempOutNode!=-1 && tempOutConnector!=-1)
	{
		bool found=false;

		i=0;
		while (i<connections.size())
		{
			if (connections[i].outNodeId==tempOutNode && connections[i].outConnectorId==tempOutConnector)
				found=true;

			i++;
		}

		if (!found)
			getNode(tempOutNode)->setConnectivityOfOutput(tempOutConnector, false);
	}

//	sortNodes();
	repaint();
}

void MainComponent::disconnectOutput(int NodeId, int OutputId)
{
	int i=0;

	while (i<(int)connections.size())
	{
		if (connections[i].outNodeId==NodeId && connections[i].outConnectorId==OutputId)
		{
			disconnectInput(connections[i].inNodeId, connections[i].inConnectorId);
			i=-1;
		}
		i++;

		getNode(NodeId)->setConnectivityOfOutput(OutputId, false);
	}

//	sortNodes();
	repaint();
}

bool MainComponent::keyPressed (const KeyPress & key, Component* originatingComponent)
{
	if (key.getKeyCode()==KeyPress::deleteKey)
	{
		const ScopedLock myScopedLock (cs);

		int selectedNodeIndex=-1;
		for (unsigned int i=0; i<nodes.size(); i++)
		{
			if (nodes[i]->isSelected())
				selectedNodeIndex=(int)i;
		}

		if (selectedNodeIndex!=-1)
			deleteNode(nodes[selectedNodeIndex]);
	}

	return true;
}

void MainComponent::mouseDown (const MouseEvent &e)
{
	if (e.mods.isLeftButtonDown() && e.eventComponent==this)
	{
		for (unsigned int i=0; i<nodes.size(); i++)
		{
			if (nodes[i]->isSelected())
				nodes[i]->setSelected(false);
		}
	}
	else if (e.mods.isLeftButtonDown() && isParentOf(e.eventComponent))
	{
		for (unsigned int i=0; i<nodes.size(); i++)
		{
			if (nodes[i]->isSelected())
				nodes[i]->setSelected(false);
		}

		if (getIndexOfChildComponent(e.eventComponent)!=-1)
			((Node*)e.eventComponent)->setSelected(true);
		else if (getIndexOfChildComponent(e.eventComponent->getParentComponent())!=-1)
			((Node*)e.eventComponent->getParentComponent())->setSelected(true);
	}
}

void MainComponent::deleteNode(Node* node)
{
	int nodeIndex=-1;
	for (unsigned int i=0; i<nodes.size(); i++)
	{
		if (nodes[i]==node)
			nodeIndex=(int)i;
	}

	if (nodeIndex!=-1)
	{
		for (int i=0; i<nodes[nodeIndex]->getNumberOfInputs(); i++)
		{
			disconnectInput(nodes[nodeIndex]->getID(), i);
		}

		for (int i=0; i<nodes[nodeIndex]->getNumberOfOutputs(); i++)
		{
			disconnectOutput(nodes[nodeIndex]->getID(), i);
		}

		oscManager.unregisterReceiver(nodes[nodeIndex]);
		removeChildComponent(nodes[nodeIndex]);
		delete nodes[nodeIndex];
		nodes.erase(nodes.begin()+nodeIndex);
	}
}

void MainComponent::deleteConnectionsOfNode(Node* node)
{
	int nodeIndex=-1;
	for (unsigned int i=0; i<nodes.size(); i++)
	{
		if (nodes[i]==node)
			nodeIndex=(int)i;
	}

	if (nodeIndex!=-1)
	{
		for (int i=0; i<nodes[nodeIndex]->getNumberOfInputs(); i++)
		{
			disconnectInput(nodes[nodeIndex]->getID(), i);
		}

		for (int i=0; i<nodes[nodeIndex]->getNumberOfOutputs(); i++)
		{
			disconnectOutput(nodes[nodeIndex]->getID(), i);
		}
	}
}

void MainComponent::sortNodes()
{
	int currentPriority = 1;
	unsigned int processedNodes=0;

	//Reset and assign the highest priority (1) to the nodes without connected inputs, and the priority of 0 for the unconnected nodes
	for (unsigned int i=0; i<nodes.size(); i++)
	{
		nodes[i]->processingPriority=numeric_limits<int>::max();

		if (!nodes[i]->isAnInputConnected() && !nodes[i]->isAnOutputConnected())
		{
			nodes[i]->processingPriority=0;
			processedNodes++;
		}
		else if (!nodes[i]->isAnInputConnected())
		{
			nodes[i]->processingPriority=currentPriority;
			processedNodes++;
		}
	}

	//Derive the priorities from the connections, until we have processed all nodes
	while (processedNodes!=nodes.size() && currentPriority<(int)nodes.size())
	{
		for (unsigned int i=0; i<connections.size(); i++)
		{
			if (getNode(connections[i].outNodeId)->processingPriority==currentPriority)
			{
				getNode(connections[i].inNodeId)->processingPriority=currentPriority+1;
				processedNodes++;
			}
		}
		currentPriority++;
	}

	sort(nodes.begin(), nodes.end(), &MainComponent::nodeSortPredicate);
}

bool MainComponent::nodeSortPredicate(const Node* a, const Node* b)
{
	return a->processingPriority < b->processingPriority;
}

void MainComponent::process() //Propagates the data over the connections and calls the process function of the nodes
{
	const ScopedLock myScopedLock (cs);

	for (unsigned int i=0; i<nodes.size(); i++)
	{
		for (unsigned int j=0; j<connections.size(); j++)
		{
			if (connections[j].inNodeId==nodes[i]->getID())
			{
				nodes[i]->setInputValue(connections[j].inConnectorId, getNode(connections[j].outNodeId)->getOutputValue(connections[j].outConnectorId));
			}
		}

		nodes[i]->process();
	}
}

void MainComponent::loadConfiguration()
{
	const ScopedLock myScopedLock (cs);

	FileChooser myChooser ("Please select the configuration file you want to load...",
                               File::getSpecialLocation (File::userHomeDirectory),
                               "*.xml");

	if (myChooser.browseForFileToOpen())
    {
        File configurationFile (myChooser.getResult());

		XmlDocument configurationDocument(configurationFile);
		XmlElement* configurationElement = configurationDocument.getDocumentElement();

		if (configurationElement->hasTagName ("Configuration"))
		{
			//Delete open configuration
			while (nodes.size()>0)
				deleteNode(nodes[0]);

			runningNodeID = 0;

			forEachXmlChildElement (*configurationElement, e)
			{
				if (e->hasTagName ("Node"))
				{
					int type = e->getIntAttribute ("type");
					if (type==INPUTNODE)
					{
						OscInputNode* node;
						node = new OscInputNode();
						node->setID(e->getIntAttribute ("id"));
						node->setOscManager(&oscManager);
						node->addMouseListener(this, true);
						
						int connectorIndex=0;
						forEachXmlChildElement (*e, ee)
						{
							if (ee->hasTagName ("Output"))
							{
								OutputConnector* outputConnector = new OutputConnector();
								outputConnector->setID(connectorIndex);
								outputConnector->setLabel(ee->getStringAttribute("label"));
								node->pushOutputConnector(outputConnector);
								node->addAndMakeVisible(outputConnector);
								connectorIndex++;
							}
						}
						
						if (connectorIndex==0)
						{
							node->setSize(100, 40);
						}
						else
							node->setSize(100, 20+connectorIndex*20);

						node->resized();
						
						node->setTopLeftPosition(e->getIntAttribute("x"), e->getIntAttribute("y"));
						node->setTitle(e->getStringAttribute("title"));
						nodes.push_back(node);
						addAndMakeVisible(node);

						String address =e->getStringAttribute("address");
						node->setAddress(address);
						int port = e->getIntAttribute("port");
						node->setPort(port);
						oscManager.registerReceiver(address, node->getNumberOfOutputs(), port, node);
					}
					else if (type == CALIBRATORNODE)
					{
						CalibratorNode* node = new CalibratorNode();
						node->initialise(e->getIntAttribute("numberOfInputs"), e->getIntAttribute("numberOfOutputs"));
						node->setID(e->getIntAttribute ("id"));
						node->addMouseListener(this, true);
						addAndMakeVisible(node);
						node->setTopLeftPosition(e->getIntAttribute("x"), e->getIntAttribute("y"));
						node->setTitle(e->getStringAttribute("title"));
						nodes.push_back(node);

						int inputIndex=0;
						int outputIndex=0;
						forEachXmlChildElement (*e, ee)
						{
							if (ee->hasTagName ("Output"))
							{
								node->getOutputConnector(outputIndex)->setLabel(ee->getStringAttribute("label"));
								node->getOutputConnector(outputIndex)->setValue((float)ee->getDoubleAttribute("value"));
								node->getConfigurator()->configureOutput(outputIndex, (float)ee->getDoubleAttribute("min"), (float)ee->getDoubleAttribute("max"), (float)ee->getDoubleAttribute("value"), ee->getBoolAttribute("minClip"),  ee->getBoolAttribute("maxClip"));
								outputIndex++;
							}
							else if (ee->hasTagName ("Input"))
							{	
								node->getInputConnector(inputIndex)->setLabel(ee->getStringAttribute("label"));
								inputIndex++;
							}
						}

						String configuration = e->getStringAttribute("calibration");
					//	node->getConfigurator()->getCalibrator()->setConfiguration(configuration.toCString());
						node->getConfigurator()->getQhullCalibrator()->setConfiguration(configuration.toCString());
					}
					else if (type == OUTPUTNODE)
					{
						OscOutputNode* node;
						node = new OscOutputNode();
						node->setID(e->getIntAttribute ("id"));
						node->addMouseListener(this, true);

						int connectorIndex=0;
						forEachXmlChildElement (*e, ee)
						{
							if (ee->hasTagName ("Input"))
							{
								InputConnector* inputConnector = new InputConnector();
								inputConnector->setID(connectorIndex);
								inputConnector->setLabel(ee->getStringAttribute("label"));
								node->pushInputConnector(inputConnector);
								node->addAndMakeVisible(inputConnector);
								connectorIndex++;
							}
						}
						
						if (connectorIndex==0)
						{
							node->setSize(100, 40);
						}
						else
							node->setSize(100, 20+connectorIndex*20);

						node->resized();
						
						node->setTopLeftPosition(e->getIntAttribute("x"), e->getIntAttribute("y"));
						node->setTitle(e->getStringAttribute("title"));
						nodes.push_back(node);
						addAndMakeVisible(node);

						node->setAddress(e->getStringAttribute("address"));
						node->setHost(e->getStringAttribute("host"));
						node->setPort(e->getIntAttribute("port"));
					}
				}
				else if (e->hasTagName ("Connection"))
				{
					connect(e->getIntAttribute("outNodeId"), e->getIntAttribute("outConnectorId"), e->getIntAttribute("inNodeId"), e->getIntAttribute("inConnectorId"));
				}

				for (unsigned int i=0; i<nodes.size(); i++)
				{
					if (runningNodeID<nodes[i]->getID())
						runningNodeID=nodes[i]->getID();
				}
			}

			//Cleaning up:
			delete configurationElement;
		}

    }
}
	
void MainComponent::saveConfiguration()
{
	FileChooser myChooser ("Please select where you want to save the configuration file...",
                               File::getSpecialLocation (File::userHomeDirectory),
                               "*.xml");

	if (myChooser.browseForFileToSave(true))
    {
        File configurationFile (myChooser.getResult());

		XmlElement configurationElement("Configuration");

		//Saving the nodes
		for(unsigned int i=0; i<nodes.size(); i++)
		{
			XmlElement* nodeElement = new XmlElement("Node");
			nodeElement->setAttribute("type", nodes[i]->getType());
			nodeElement->setAttribute("id", nodes[i]->getID());
			nodeElement->setAttribute("x", nodes[i]->getPosition().getX());
			nodeElement->setAttribute("y", nodes[i]->getPosition().getY());
			nodeElement->setAttribute("title", nodes[i]->getTitle());

			if (nodes[i]->getType()==INPUTNODE)
			{
				nodeElement->setAttribute("address", ((OscInputNode*)nodes[i])->getAddress());
				nodeElement->setAttribute("port", ((OscInputNode*)nodes[i])->getPort());

				//Save the connector names
				for (int ii=0; ii<nodes[i]->getNumberOfOutputs(); ii++)
				{
					XmlElement* connectorElement = new XmlElement("Output");
					connectorElement->setAttribute("label", ((OscInputNode*)nodes[i])->getOutputConnector(ii)->getLabel());
					nodeElement->addChildElement(connectorElement);
				}
			}
			else if (nodes[i]->getType()==CALIBRATORNODE)
			{
				nodeElement->setAttribute("numberOfInputs", nodes[i]->getNumberOfInputs());
				nodeElement->setAttribute("numberOfOutputs", nodes[i]->getNumberOfOutputs());

				//Save the connector names
				for (int ii=0; ii<nodes[i]->getNumberOfInputs(); ii++)
				{
					XmlElement* connectorElement = new XmlElement("Input");
					connectorElement->setAttribute("label", nodes[i]->getInputConnector(ii)->getLabel());
					nodeElement->addChildElement(connectorElement);
				}

				for (int ii=0; ii<nodes[i]->getNumberOfOutputs(); ii++)
				{
					XmlElement* connectorElement = new XmlElement("Output");
					connectorElement->setAttribute("label", nodes[i]->getOutputConnector(ii)->getLabel());
					connectorElement->setAttribute("value", nodes[i]->getOutputConnector(ii)->getValue());
					connectorElement->setAttribute("min", ((CalibratorNode*)nodes[i])->getOutputMin(ii));
					connectorElement->setAttribute("max", ((CalibratorNode*)nodes[i])->getOutputMax(ii));
					connectorElement->setAttribute("minClip", ((CalibratorNode*)nodes[i])->getMinClip(ii));
					connectorElement->setAttribute("maxClip", ((CalibratorNode*)nodes[i])->getMaxClip(ii));
					nodeElement->addChildElement(connectorElement);
				}

				String calibration = ((CalibratorNode*)nodes[i])->getConfigurator()->getQhullCalibrator()->getConfiguration().c_str();
				nodeElement->setAttribute("calibration", calibration);

			}
			else if (nodes[i]->getType()==OUTPUTNODE)
			{
				nodeElement->setAttribute("address", ((OscOutputNode*)nodes[i])->getAddress());
				nodeElement->setAttribute("port", ((OscOutputNode*)nodes[i])->getPort());
				nodeElement->setAttribute("host", ((OscOutputNode*)nodes[i])->getHost());

				//Save the connector names
				for (int ii=0; ii<nodes[i]->getNumberOfInputs(); ii++)
				{
					XmlElement* connectorElement = new XmlElement("Input");
					connectorElement->setAttribute("label", ((OscInputNode*)nodes[i])->getInputConnector(ii)->getLabel());
					nodeElement->addChildElement(connectorElement);
				}
			}

			configurationElement.addChildElement(nodeElement);
		}

		for(unsigned int i=0; i<connections.size(); i++)
		{
			XmlElement* connectionElement = new XmlElement("Connection");
			connectionElement->setAttribute("inConnectorId", connections[i].inConnectorId);
			connectionElement->setAttribute("inNodeId", connections[i].inNodeId);
			connectionElement->setAttribute("outConnectorId", connections[i].outConnectorId);
			connectionElement->setAttribute("outNodeId", connections[i].outNodeId);

			configurationElement.addChildElement(connectionElement);
		}

		if (configurationFile.hasFileExtension("xml"))
			configurationFile.replaceWithText(configurationElement.createDocument("OscCalibrator Configuration File"));
		else
		{
			File configurationFileWithExtension(configurationFile.getFullPathName()+".xml");
			configurationFileWithExtension.replaceWithText(configurationElement.createDocument("OscCalibrator Configuration File"));
		}
    }
}
