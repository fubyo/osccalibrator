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


#include "CalibratorConfigurator.h"
#include "CalibratorNode.h"
#include "MainComponent.h"

CalibratorConfigurator::CalibratorConfigurator (int NumberOfInputs, int NumberOfOutputs, CalibratorNode* theCalibratorNode)
    : addButton (0),
      label (0),
      numberLabel (0),
      clearButton (0)
{

	calibratorNode = theCalibratorNode;
	numberOfInputs=NumberOfInputs;
	numberOfOutputs=NumberOfOutputs;

	for (int i=0; i<numberOfOutputs; i++)
	{
		ParameterSlider *parameterSlider = new ParameterSlider();
		parameterSliders.push_back(parameterSlider);
		addAndMakeVisible (parameterSlider);
	}

//	calibrator = new Calibrator(numberOfInputs, numberOfOutputs);
	qhullCalibrator = new QhullCalibrator(numberOfInputs, numberOfOutputs);

	for (int i=0; i<numberOfInputs; i++)
		inputValues.push_back(0);

	for (int i=0; i<numberOfOutputs; i++)
		outputValues.push_back(0);

    addAndMakeVisible (addButton = new TextButton (T("new button")));
    addButton->setButtonText (T("Add calibration point"));
    addButton->addListener (this);
    addButton->setColour (TextButton::buttonColourId, Colour (0xffd8e2cf));
    addButton->setColour (TextButton::buttonOnColourId, Colour (0xffbcb08d));

    addAndMakeVisible (label = new Label (T("new label"),
                                          T("Number of calibration points:")));
    label->setFont (Font (15.0000f, Font::plain));
    label->setJustificationType (Justification::centredLeft);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (numberLabel = new Label (T("new label"),
                                                T("0")));
    numberLabel->setFont (Font (15.0000f, Font::plain));
    numberLabel->setJustificationType (Justification::centredLeft);
    numberLabel->setEditable (false, false, false);
    numberLabel->setColour (TextEditor::textColourId, Colours::black);
    numberLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (clearButton = new TextButton (T("new button")));
    clearButton->setButtonText (T("Clear calibration"));
    clearButton->addListener (this);
    clearButton->setColour (TextButton::buttonColourId, Colour (0xffe25f0b));
    clearButton->setColour (TextButton::buttonOnColourId, Colour (0xffe94b24));


	addButton->addMouseListener(this, false);
	clearButton->addMouseListener(this, false);



    //[UserPreSize]
    //[/UserPreSize]

    setSize (400, 70+50*NumberOfOutputs);

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]

	
}

CalibratorConfigurator::~CalibratorConfigurator()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    for (int i=0; i<numberOfOutputs; i++)
	{
		deleteAndZero(parameterSliders[i]);
	}

    deleteAndZero (addButton);
    deleteAndZero (label);
    deleteAndZero (numberLabel);
    deleteAndZero (clearButton);

//	delete calibrator;
	delete qhullCalibrator;

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CalibratorConfigurator::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::grey);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CalibratorConfigurator::resized()
{
    addButton->setBounds (244, 8, 150, 24);
	clearButton->setBounds (6, 8, 106, 24);

	for (int i=0; i<numberOfOutputs; i++)
		parameterSliders[i]->setBounds (0, 41+i*50, 400, 50);

    label->setBounds (0, 45+numberOfOutputs*50, 184, 24);
    numberLabel->setBounds (192,  45+numberOfOutputs*50, 104, 24);
   
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CalibratorConfigurator::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == addButton)
    {
		vector<double> point;
		for (int i=0; i<numberOfInputs; i++)
		{
			point.push_back((double)inputValues[i]);
		}

		vector<double> configuration;
		for (unsigned int i=0; i<parameterSliders.size(); i++)
		{
			configuration.push_back(parameterSliders[i]->getValue());
		}

		CalibrationPoint cp(point, configuration);
		//calibrator->addCalibrationPoint(cp);
		qhullCalibrator->addCalibrationPoint(cp);
		qhullCalibrator->tryToPerformTriangulation();

		numberLabel->setText(String(qhullCalibrator->getNumberOfCalibrationPoints()),false);
    }
    else if (buttonThatWasClicked == clearButton)
    {
        if (qhullCalibrator)
		{
		//	delete calibrator;
		//	calibrator = new Calibrator(numberOfInputs, numberOfOutputs);

			delete qhullCalibrator;
			qhullCalibrator = new QhullCalibrator(numberOfInputs, numberOfOutputs);
		}

		numberLabel->setText(String(qhullCalibrator->getNumberOfCalibrationPoints()),false);
    }
}


void CalibratorConfigurator::setParameterLabel(int Index, String Text)
{
	if (Index>=0 && Index<(int)parameterSliders.size())
		parameterSliders[Index]->setLabel(Text);
}

void CalibratorConfigurator::setInputValue(int Index, float Value)
{
	if (Index>=0 && Index<numberOfInputs)
		inputValues[Index]=Value;
}

float CalibratorConfigurator::getOutputValue(int Index)
{
	if (Index>=0 && Index<numberOfOutputs)
		return outputValues[Index];
	else return 0;
}

void CalibratorConfigurator::process()
{
	if (isShowing())
	{
		for (unsigned int i=0; i<parameterSliders.size(); i++)
		{
			if (parameterSliders[i]->remoteControl)
			{
				parameterSliders[i]->setValue(parameterSliders[i]->remoteNode->getOutputValue(parameterSliders[i]->remoteOutputIndex));
				outputValues[i] = parameterSliders[i]->remoteNode->getOutputValue(parameterSliders[i]->remoteOutputIndex);
			}
			else
				outputValues[i] =(float)parameterSliders[i]->getValue();
		}
	}
	else
	{
		vector<double> point;
		for (int i=0; i<numberOfInputs; i++)
		{
			point.push_back((double)inputValues[i]);
		}
		
		vector<double> result;
		result = qhullCalibrator->getInterpolated(point);

		if (result.size())
			for (int i=0; i<numberOfOutputs; i++)
			{
				if (parameterSliders[i]->getMinClip() && result[i]<parameterSliders[i]->getMin())
					result[i] = parameterSliders[i]->getMin();

				if (parameterSliders[i]->getMaxClip() && result[i]>parameterSliders[i]->getMax())
					result[i] = parameterSliders[i]->getMax();

				outputValues[i] = (float)result[i];
			}
	}
}

void CalibratorConfigurator::configureOutput(int index, float min, float max, float value, bool minClip, bool maxClip)
{
	setRange(index, min, max, minClip, maxClip);
	setOutputValue(index, value); 
}

void CalibratorConfigurator::configureRemote(int ParameterIndex, bool RemoteControl, Node* RemoteNode, int RemoteOutputIndex)
{
	parameterSliders[ParameterIndex]->remoteControl = RemoteControl;
	parameterSliders[ParameterIndex]->remoteNode = RemoteNode;
	parameterSliders[ParameterIndex]->remoteOutputIndex = RemoteOutputIndex;
}

void CalibratorConfigurator::mouseDrag(const MouseEvent &e)
{
	if (e.mods.isLeftButtonDown())
	{
		if (e.eventComponent==addButton)
			startDragging(String("addremote"), e.eventComponent , Image::null, true);
		else if (e.eventComponent==clearButton)
			startDragging(String("clearremote"), e.eventComponent , Image::null, true);
	}
}

void CalibratorConfigurator::mouseUp (const MouseEvent &e)
{
	if (e.mods.isRightButtonDown())
	{
		PopupMenu m;
		m.addItem (1, "Disable remote controlling");
        const int result = m.show();

		if (result==1)
		{
			setRemoteAdding(false);
			setRemoteClearing(false);
			MainComponent* mainComponent = (MainComponent*)calibratorNode->getParentComponent();
			mainComponent->oscManager.setRemoteAdding(false, String(), 0);
			mainComponent->oscManager.setRemoteClearing(false, String(), 0);
		}
	}
}

void CalibratorConfigurator::setRemoteAdding(bool State)
{
	if (State)
		addButton->setColour (TextButton::buttonColourId, Colour (0xffffe2cf));
	else
		addButton->setColour (TextButton::buttonColourId, Colour (0xffd8e2cf));
}

void CalibratorConfigurator::setRemoteClearing(bool State)
{
	if (State)
		clearButton->setColour (TextButton::buttonColourId, Colour (0xffefff0b));
	else
		clearButton->setColour (TextButton::buttonColourId, Colour (0xffe25f0b));
}