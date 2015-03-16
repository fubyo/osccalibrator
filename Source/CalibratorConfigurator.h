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
#include "ParameterSlider.h"
#include "QhullCalibrator\QhullCalibrator.h"
#include "Node.h"
#include "OscManager.h"

#include "Pool.h"

#include <vector>
using namespace std;

class CalibratorNode;

class CalibratorConfigurator  : public Component,
                                public ButtonListener,
								public DragAndDropContainer
{
public:
    //==============================================================================
    CalibratorConfigurator (int NumberOfInputs, int NumberOfOutputs, CalibratorNode* theCalibratorNode);
    ~CalibratorConfigurator();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);

	void setParameterLabel(int Index, String Text);

	void setInputValue(int Index, float Value);
	float getOutputValue(int Index);

	void process();

	CalibratorNode* getCalibratorNode() {return calibratorNode;};
	float getOutputMin(int index) {return (float)parameterSliders[index]->getMin();};
	float getOutputMax(int index) {return (float)parameterSliders[index]->getMax();};
	void setOutputValue(int index, float value) {parameterSliders[index]->setValue((double)value);};
	void setCalibrationPointsNumber(int number) {numberLabel->setText(String(number), false);};

	bool getMinClip(int index) {return parameterSliders[index]->getMinClip();};
	bool getMaxClip(int index) {return parameterSliders[index]->getMaxClip();};

//	Calibrator* getCalibrator() {return calibrator;};
	QhullCalibrator* getQhullCalibrator() {return qhullCalibrator;};

	void configureOutput(int index, float min, float max, float value, bool minClip, bool maxClip);

	void configureRemote(int ParameterIndex, bool RemoteControl, Node* RemoteNode, int RemoteOutputIndex);

	void setRemoteAdding(bool State);
	void setRemoteClearing(bool State);
	
	vector<ParameterSlider*> parameterSliders;

	void mouseDrag (const MouseEvent &e);
	void mouseUp (const MouseEvent &e);

	TextButton* clearButton;
    TextButton* addButton;
    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
	
    Label* label;
    Label* numberLabel;
    
	int numberOfInputs;
	int numberOfOutputs;

	//Calibrator* calibrator;
	QhullCalibrator* qhullCalibrator;
	vector<float> inputValues;
	vector<float> outputValues;
	
	CalibratorNode* calibratorNode;
	
	void setRange(int index, float min, float max, bool minClip, bool maxClip) {parameterSliders[index]->setRange(min, max, minClip, maxClip);};

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    CalibratorConfigurator (const CalibratorConfigurator&);
    const CalibratorConfigurator& operator= (const CalibratorConfigurator&);
};
