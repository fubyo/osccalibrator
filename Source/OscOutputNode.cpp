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


#include "OscOutputNode.h"
#include "MainComponent.h"

OscOutputNode::OscOutputNode ()
{
	title->setText("Osc output", false);
	title->addMouseListener(title->getParentComponent(), false);

	oscManager=(OscManager*)Pool::Instance()->getObject("OscManager");
	type=OUTPUTNODE;

	host = "127.0.0.1";
	port = 3334;
	address = "/address";

	//transmitSocket = 0;
}

OscOutputNode::~OscOutputNode()
{
	oscManager->deleteTransmitSocket(host, port);
	
	/*if (transmitSocket)
	{
	 	delete transmitSocket;
	}*/
}

void OscOutputNode::mouseDoubleClick(const MouseEvent &e)
{
	if (e.mods.isLeftButtonDown() && e.eventComponent==this)
	{
		OscOutputConfigurator* configurator = new OscOutputConfigurator();
		configurator->setHost(getHost());
		configurator->setAddress(getAddress());
		configurator->setPort(getPort());
		configurator->setNumberOfParameters(getNumberOfInputs());


		DialogWindow::showModalDialog(title->getText()+" configuration", configurator, this, Colours::lightgrey, false);
		
		if (configurator->okClicked)
		{
			if (configurator->getNumberOfParameters() != getNumberOfInputs())
			{
				((MainComponent*)e.eventComponent->getParentComponent())->deleteConnectionsOfNode((Node*)e.eventComponent);

				for (unsigned int i=0; i<inputConnectors.size(); i++)
					deleteAndZero(inputConnectors[i]);
				inputConnectors.clear();

				for (int i=0; i<configurator->getNumberOfParameters(); i++)
				{
					InputConnector* inputConnector = new InputConnector();
					inputConnector->setID(i);
					addAndMakeVisible(inputConnector);
					inputConnectors.push_back(inputConnector);
				}

				if (inputConnectors.size()==0)
				{
					setSize(100, 40);
				}
				else
					setSize(100, 20+inputConnectors.size()*20);
			}

			setHost(configurator->getHost());
			setPort(configurator->getPort());
			address=configurator->getAddress();

			resized();
		}
		
		delete configurator;
	}
}

void OscOutputNode::process()
{
	vector<float> parameters;

	for (unsigned int i=0; i<inputConnectors.size(); i++)
	{
		parameters.push_back(inputConnectors[i]->getValue());
	}

	
/*	if (transmitSocket)
	{
		osc::OutboundPacketStream p(buffer, 1024);
    
		p << osc::BeginMessage( address.toCString() );

		for (unsigned int i=0; i < parameters.size(); i++)
		{
			p << parameters[i];
		}

		p << osc::EndMessage;
    
		transmitSocket->Send( p.Data(), p.Size() );
	} */
	
	oscManager->sendOSC(host, port, address, parameters);
}

void OscOutputNode::setPort(int Port)
{
	port = Port;

/*	if (transmitSocket)
		delete transmitSocket;

	transmitSocket = new UdpTransmitSocket(IpEndpointName( host.toCString(), port)); */ 
}


void OscOutputNode::setHost(String Host)
{
	host = Host;

/*	if (transmitSocket)
		delete transmitSocket;

	transmitSocket = new UdpTransmitSocket(IpEndpointName( host.toCString(), port)); */
}
