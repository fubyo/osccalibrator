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


#include "OscInputNode.h"
#include "MainComponent.h"

OscInputNode::OscInputNode ()
{
	title->setText("Osc input", false);
	title->addMouseListener(title->getParentComponent(), false);

	oscManager=0;
	type=INPUTNODE;
	address="/address";
	port=3333;
}

OscInputNode::~OscInputNode()
{
	
}

void OscInputNode::mouseDoubleClick(const MouseEvent &e)
{
	if (e.mods.isLeftButtonDown() && e.eventComponent==this)
	{
		OscInputConfigurator* configurator = new OscInputConfigurator();
		configurator->setAddress(getAddress());
		configurator->setPort(getPort());
		configurator->setNumberOfParameters(getNumberOfOutputs());

		DialogWindow::showModalDialog(title->getText()+" configuration", configurator, this, Colours::lightgrey, false);

		if (configurator->okClicked)
		{
			if (configurator->getNumberOfParameters() != getNumberOfOutputs())
			{
				((MainComponent*)e.eventComponent->getParentComponent())->deleteConnectionsOfNode((Node*)e.eventComponent);

				for (unsigned int i=0; i<outputConnectors.size(); i++)
					deleteAndZero(outputConnectors[i]);
				outputConnectors.clear();

				for (int i=0; i<configurator->getNumberOfParameters(); i++)
				{
					OutputConnector* outputConnector = new OutputConnector();
					outputConnector->setID(i);
					addAndMakeVisible(outputConnector);
					outputConnectors.push_back(outputConnector);
				}

				if (outputConnectors.size()==0)
				{
					setSize(100, 40);
				}
				else
					setSize(100, 20+outputConnectors.size()*20);
			}

			if (configurator->getAddress() != getAddress() || configurator->getPort() != getPort())
			{
				oscManager->unregisterReceiver(this);

				//Register the osc address
				address = configurator->getAddress();
				port = configurator->getPort();
				oscManager->registerReceiver(address, configurator->getNumberOfParameters(), port, this);
			}

			resized();
		}
		
		delete configurator;
	}
}
