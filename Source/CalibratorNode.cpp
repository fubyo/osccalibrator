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


#include "CalibratorNode.h"
#include "MainComponent.h"

CalibratorNode::CalibratorNode ()
{
	title->setText("Calibrator", false);
	title->addMouseListener(title->getParentComponent(), false);

	configurator=0;
	type=CALIBRATORNODE;
}

CalibratorNode::~CalibratorNode()
{
	if (configurator)
		delete configurator;
}

void CalibratorNode::initialise(int NumberOfInputs, int NumberOfOutputs)
{
	for (unsigned int i=0; i<outputConnectors.size(); i++)
			deleteAndZero(outputConnectors[i]);
		
	outputConnectors.clear();

	for (unsigned int i=0; i<inputConnectors.size(); i++)
			deleteAndZero(inputConnectors[i]);
		
	inputConnectors.clear();

	for (int i=0; i<NumberOfInputs; i++)
	{
		InputConnector* inputConnector = new InputConnector();
		inputConnector->setID(i);
		addAndMakeVisible(inputConnector);
		inputConnectors.push_back(inputConnector);
	}

	for (int i=0; i<NumberOfOutputs; i++)
			{
				OutputConnector* outputConnector = new OutputConnector();
				outputConnector->setID(i);
				addAndMakeVisible(outputConnector);
				outputConnectors.push_back(outputConnector);
			}

	if (outputConnectors.size()==0 && inputConnectors.size()==0)
	{
		setSize(100, 40);
	}
	else
		setSize(100, 20+outputConnectors.size()*20+inputConnectors.size()*20);


	configurator = new CalibratorConfigurator(NumberOfInputs, NumberOfOutputs, this);
}

void CalibratorNode::mouseDoubleClick(const MouseEvent &e)
{
	if (e.mods.isLeftButtonDown() && e.eventComponent==this)
	{
		for (int i=0; i<(int)outputConnectors.size(); i++)
		{
			configurator->setParameterLabel(i, outputConnectors[i]->getLabel());
			configurator->setOutputValue(i, outputConnectors[i]->getValue());
		}

		configurator->setCalibrationPointsNumber(configurator->getQhullCalibrator()->getNumberOfCalibrationPoints());

		MainComponent* mainComponent = (MainComponent*)getParentComponent();
		mainComponent->activeConfigurator = configurator;
		DialogWindow::showModalDialog(title->getText()+" configuration", configurator, this, Colours::lightgrey, false);
		mainComponent->activeConfigurator = 0;

		for (int i = 0; i < this->getNumberOfOutputs(); i++)
		{
			configurator->parameterSliders[i]->remoteControl = false;
			configurator->parameterSliders[i]->remoteNode = 0;
			configurator->parameterSliders[i]->remoteOutputIndex = -1;
		}

		mainComponent->repaint();
	}
}

void CalibratorNode::process()
{
	for (unsigned int i=0; i<inputConnectors.size(); i++)
		configurator->setInputValue(i, inputConnectors[i]->getValue());

	configurator->process();

	for (unsigned int i=0; i<outputConnectors.size(); i++)
		outputConnectors[i]->setValue(configurator->getOutputValue(i));
}

float CalibratorNode::getOutputMin(int index)
{
	return configurator->getOutputMin(index);
}
	
float CalibratorNode::getOutputMax(int index)
{
	return configurator->getOutputMax(index);
}

bool CalibratorNode::getMinClip(int index)
{
	return configurator->getMinClip(index);
}
	
bool CalibratorNode::getMaxClip(int index)
{
	return configurator->getMaxClip(index);
}