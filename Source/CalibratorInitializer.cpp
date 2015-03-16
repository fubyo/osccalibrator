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


#include "CalibratorInitializer.h"

//==============================================================================
CalibratorInitializer::CalibratorInitializer ()
    : label (0),
      label2 (0),
      numberOfInputsEditor (0),
      numberOfOutputsEditor (0),
      acceptButton (0),
      cancelButton (0)
{
    addAndMakeVisible (label = new Label (T("new label"),
                                          T("Number of inputs:")));
    label->setFont (Font (15.0000f, Font::plain));
    label->setJustificationType (Justification::centredRight);
    label->setEditable (false, false, false);
    label->setColour (TextEditor::textColourId, Colours::black);
    label->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (label2 = new Label (T("new label"),
                                           T("Number of outputs:")));
    label2->setFont (Font (15.0000f, Font::plain));
    label2->setJustificationType (Justification::centredLeft);
    label2->setEditable (false, false, false);
    label2->setColour (TextEditor::textColourId, Colours::black);
    label2->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (numberOfInputsEditor = new TextEditor (T("new text editor")));
    numberOfInputsEditor->setMultiLine (false);
    numberOfInputsEditor->setReturnKeyStartsNewLine (false);
    numberOfInputsEditor->setReadOnly (false);
    numberOfInputsEditor->setScrollbarsShown (true);
    numberOfInputsEditor->setCaretVisible (true);
    numberOfInputsEditor->setPopupMenuEnabled (true);
    numberOfInputsEditor->setText (T("1"));

    addAndMakeVisible (numberOfOutputsEditor = new TextEditor (T("new text editor")));
    numberOfOutputsEditor->setMultiLine (false);
    numberOfOutputsEditor->setReturnKeyStartsNewLine (false);
    numberOfOutputsEditor->setReadOnly (false);
    numberOfOutputsEditor->setScrollbarsShown (true);
    numberOfOutputsEditor->setCaretVisible (true);
    numberOfOutputsEditor->setPopupMenuEnabled (true);
    numberOfOutputsEditor->setText (T("1"));

    addAndMakeVisible (acceptButton = new TextButton (T("new button")));
    acceptButton->setButtonText (T("Accept"));
    acceptButton->addListener (this);
    acceptButton->setColour (TextButton::buttonColourId, Colour (0xff63eb65));

    addAndMakeVisible (cancelButton = new TextButton (T("new button")));
    cancelButton->setButtonText (T("Cancel"));
    cancelButton->addListener (this);
    cancelButton->setColour (TextButton::buttonColourId, Colour (0xfff28686));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (200, 80);

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

CalibratorInitializer::~CalibratorInitializer()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    deleteAndZero (label);
    deleteAndZero (label2);
    deleteAndZero (numberOfInputsEditor);
    deleteAndZero (numberOfOutputsEditor);
    deleteAndZero (acceptButton);
    deleteAndZero (cancelButton);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void CalibratorInitializer::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::aliceblue);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void CalibratorInitializer::resized()
{
    label->setBounds (1, 0, 128, 24);
    label2->setBounds (1, 24, 128, 24);
    numberOfInputsEditor->setBounds (129, 4, 64, 16);
    numberOfOutputsEditor->setBounds (129, 28, 64, 16);
    acceptButton->setBounds (41, 52, 72, 24);
    cancelButton->setBounds (121, 52, 72, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void CalibratorInitializer::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == acceptButton)
    {
		okClicked=true;
		getParentComponent()->exitModalState(1);
    }
    else if (buttonThatWasClicked == cancelButton)
    {
		okClicked=false;
		getParentComponent()->exitModalState(0);
    }
}

int  CalibratorInitializer::getNumberOfInputs()
{
	return numberOfInputsEditor->getText().getIntValue();
}

int  CalibratorInitializer::getNumberOfOutputs()
{
	return numberOfOutputsEditor->getText().getIntValue();
}


