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


#include "ParameterSlider.h"
#include "CalibratorConfigurator.h"
#include "CalibratorNode.h"
#include "MainComponent.h"


//==============================================================================
ParameterSlider::ParameterSlider ()
    : slider (0),
      paramLabel (0),
      minEditor (0),
      toLabel (0),
      maxEditor (0),
      fromLabel (0)
{
    addAndMakeVisible (slider = new Slider (T("new slider")));
    slider->setRange (0, 1, 0);
    slider->setSliderStyle (Slider::LinearHorizontal);
    slider->setTextBoxStyle (Slider::TextBoxLeft, false, 80, 20);
    slider->addListener (this);

    addAndMakeVisible (paramLabel = new Label (T("new label"),
                                               T("out")));
    paramLabel->setFont (Font (15.0000f, Font::plain));
    paramLabel->setJustificationType (Justification::centredLeft);
    paramLabel->setEditable (false, false, false);
    paramLabel->setColour (TextEditor::textColourId, Colours::black);
    paramLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));
	paramLabel->addMouseListener(this, false);

    addAndMakeVisible (minEditor = new TextEditor (T("new text editor")));
    minEditor->setMultiLine (false);
    minEditor->setReturnKeyStartsNewLine (false);
    minEditor->setReadOnly (false);
    minEditor->setScrollbarsShown (true);
    minEditor->setCaretVisible (true);
    minEditor->setPopupMenuEnabled (true);
    minEditor->setText (T("0"));
	minEditor->addListener(this);

    addAndMakeVisible (toLabel = new Label (T("new label"),
                                           T("to")));
    toLabel->setFont (Font (15.0000f, Font::plain));
    toLabel->setJustificationType (Justification::centredLeft);
    toLabel->setEditable (false, false, false);
    toLabel->setColour (TextEditor::textColourId, Colours::black);
    toLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (maxEditor = new TextEditor (T("new text editor")));
    maxEditor->setMultiLine (false);
    maxEditor->setReturnKeyStartsNewLine (false);
    maxEditor->setReadOnly (false);
    maxEditor->setScrollbarsShown (true);
    maxEditor->setCaretVisible (true);
    maxEditor->setPopupMenuEnabled (true);
    maxEditor->setText (T("1"));
	maxEditor->addListener(this);

    addAndMakeVisible (fromLabel = new Label (T("new label"),
                                              T("from")));
    fromLabel->setFont (Font (15.0000f, Font::plain));
    fromLabel->setJustificationType (Justification::centredLeft);
    fromLabel->setEditable (false, false, false);
    fromLabel->setColour (TextEditor::textColourId, Colours::black);
    fromLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

	addAndMakeVisible (minClipToggle = new ToggleButton ());
    minClipToggle->setButtonText (T("clip"));

	addAndMakeVisible (maxClipToggle = new ToggleButton ());
    maxClipToggle->setButtonText (T("clip"));
    //[UserPreSize]
    //[/UserPreSize]

    setSize (400, 50);

	remoteControl = false;
	remoteNode = 0;
	remoteOutputIndex = -1;

	sliderChangedRemotely = false;
    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ParameterSlider::~ParameterSlider()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (slider);
    deleteAndZero (paramLabel);
    deleteAndZero (minEditor);
    deleteAndZero (toLabel);
    deleteAndZero (maxEditor);
    deleteAndZero (fromLabel);
	deleteAndZero (minClipToggle);
	deleteAndZero (maxClipToggle);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ParameterSlider::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

	if (remoteControl)
		g.fillAll (Colour (0xffcffcd9));
	else
		g.fillAll (Colour (0xffcddcd9));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ParameterSlider::resized()
{
    slider->setBounds (24, 24, 372, 24);
    paramLabel->setBounds (0, 0, 112, 24);

    fromLabel->setBounds (132, 0, 40, 24);
	minEditor->setBounds (173, 3, 56, 16);
	minClipToggle->setBounds(230, 3, 40, 16);

	toLabel->setBounds (275, 0, 24, 24);
    maxEditor->setBounds (300, 3, 56, 16);
	maxClipToggle->setBounds(357, 3, 40, 16);
	
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ParameterSlider::sliderValueChanged (Slider* sliderThatWasMoved)
{
    if (sliderThatWasMoved == slider)
    {
		if (!sliderChangedRemotely)
			((MainComponent*)((CalibratorConfigurator*)getParentComponent())->getCalibratorNode()->getParentComponent())->process();
    }
}

void ParameterSlider::textEditorTextChanged (TextEditor &editor)
{
	slider->setRange(minEditor->getText().getFloatValue(), maxEditor->getText().getFloatValue(), 0);
}

void ParameterSlider::textEditorReturnKeyPressed (TextEditor &editor)
{

}

void ParameterSlider::textEditorEscapeKeyPressed (TextEditor &editor)
{
}

void ParameterSlider::textEditorFocusLost (TextEditor &editor)
{
}

void ParameterSlider::setLabel(String Text)
{
	paramLabel->setText(Text, true);
}

void ParameterSlider::setRange(double min, double max, bool minClip, bool maxClip)
{
	slider->setRange(min, max);
	minEditor->setText(String(min));
	maxEditor->setText(String(max));
	minClipToggle->setToggleState(minClip, false);
	maxClipToggle->setToggleState(maxClip, false);
}

void ParameterSlider::mouseDrag (const MouseEvent &e)
{
	CalibratorConfigurator* configurator= (CalibratorConfigurator*)getParentComponent();
	vector<ParameterSlider*> sliders = configurator->parameterSliders;

	//find the index of the parameter
	int index = -1;
	for (unsigned int i = 0; i<sliders.size(); i++)
		if (sliders[i] == this)
			index = i;

	if (e.mods.isLeftButtonDown())
	{
		startDragging(String("calibratorremote")+" "+String(index), this, Image::null, true);
	}
}

void ParameterSlider::mouseUp (const MouseEvent &e)
{
	if (e.mods.isRightButtonDown())
	{
		PopupMenu m;
		m.addItem (1, "Disable remote controlling");
        const int result = m.show();

		if (result==1)
		{
			remoteControl=false;
			remoteNode=0;
			remoteOutputIndex=-1;
			repaint();
		}
	}
}

void ParameterSlider::setValue(float Value)
{
	float min = minEditor->getText().getFloatValue();
	float max = maxEditor->getText().getFloatValue();
	bool minClip = minClipToggle->getToggleState();
	bool maxClip = maxClipToggle->getToggleState();

	if (!minClip)
		if (Value<=min)
		{
			min = Value;
		}
	else
		if (Value<=min)
		{
			Value = min;
		}

	if (!maxClip)
		if (Value>=max)
		{
			max = Value;
		}
	else
		if (Value>=max)
		{
			Value = max;
		}

	const MessageManagerLock mmLock;

	sliderChangedRemotely = true;

	this->setRange(min, max, minClip, maxClip);
	slider->setValue(Value);
}