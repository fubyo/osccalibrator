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
#include "Node.h"


class ParameterSlider  : public Component,
                         public SliderListener,
						 public TextEditorListener,
						 public DragAndDropContainer 
{
public:
    //==============================================================================
    ParameterSlider ();
    ~ParameterSlider();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);

	void textEditorTextChanged (TextEditor &editor);
	void textEditorReturnKeyPressed (TextEditor &editor);
	void textEditorEscapeKeyPressed (TextEditor &editor);
	void textEditorFocusLost (TextEditor &editor);

	void setLabel(String Text);
	double getValue() {return slider->getValue();};
	void setValue(float Value);

	bool getMinClip() {return minClipToggle->getToggleState(); };
	bool getMaxClip() {return maxClipToggle->getToggleState(); };

	double getMin() {return slider->getMinimum();};
	double getMax() {return slider->getMaximum();};
	void setRange(double min, double max, bool minClip, bool maxClip); 
	void setValue(double value)
	{
		sliderChangedRemotely = false;
		slider->setValue(value);
	};

	void mouseDrag (const MouseEvent &e);
	void mouseUp (const MouseEvent &e);

	bool remoteControl;
	Node* remoteNode;
	int remoteOutputIndex;

	bool sliderChangedRemotely;

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    Slider* slider;
    Label* paramLabel;
    TextEditor* minEditor;
    Label* toLabel;
    TextEditor* maxEditor;
    Label* fromLabel;
	ToggleButton* minClipToggle;
	ToggleButton* maxClipToggle;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    ParameterSlider (const ParameterSlider&);
    const ParameterSlider& operator= (const ParameterSlider&);
};

