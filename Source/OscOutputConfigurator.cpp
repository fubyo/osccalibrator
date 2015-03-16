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


#include "OscOutputConfigurator.h"

OscOutputConfigurator::OscOutputConfigurator ()
    : hostLabel (0),
	  hostEditor (0),
	  addressLabel (0),
      addressEditor (0),
      portlabel (0),
      portEditor (0),
      parameterslabel (0),
      parametersEditor (0),
      okButton (0),
      cancelButton (0)
{
	addAndMakeVisible (hostLabel = new Label (T("host"),
                                                 T("Host:")));
    hostLabel->setFont (Font (15.0000f, Font::plain));
    hostLabel->setJustificationType (Justification::centredLeft);
    hostLabel->setEditable (false, false, false);
    hostLabel->setColour (TextEditor::textColourId, Colours::black);
    hostLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (hostEditor = new TextEditor (String::empty));
    hostEditor->setMultiLine (false);
    hostEditor->setReturnKeyStartsNewLine (false);
    hostEditor->setReadOnly (false);
    hostEditor->setScrollbarsShown (true);
    hostEditor->setCaretVisible (true);
    hostEditor->setPopupMenuEnabled (true);
    hostEditor->setText ("127.0.0.1");

    addAndMakeVisible (addressLabel = new Label (T("address"),
                                                 T("Address:")));
    addressLabel->setFont (Font (15.0000f, Font::plain));
    addressLabel->setJustificationType (Justification::centredLeft);
    addressLabel->setEditable (false, false, false);
    addressLabel->setColour (TextEditor::textColourId, Colours::black);
    addressLabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (addressEditor = new TextEditor (String::empty));
    addressEditor->setMultiLine (false);
    addressEditor->setReturnKeyStartsNewLine (false);
    addressEditor->setReadOnly (false);
    addressEditor->setScrollbarsShown (true);
    addressEditor->setCaretVisible (true);
    addressEditor->setPopupMenuEnabled (true);
    addressEditor->setText ("/address");

    addAndMakeVisible (portlabel = new Label (T("port"),
                                              T("Port:")));
    portlabel->setFont (Font (15.0000f, Font::plain));
    portlabel->setJustificationType (Justification::centredLeft);
    portlabel->setEditable (false, false, false);
    portlabel->setColour (TextEditor::textColourId, Colours::black);
    portlabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (portEditor = new TextEditor (String::empty));
    portEditor->setMultiLine (false);
    portEditor->setReturnKeyStartsNewLine (false);
    portEditor->setReadOnly (false);
    portEditor->setScrollbarsShown (true);
    portEditor->setCaretVisible (true);
    portEditor->setPopupMenuEnabled (true);
    portEditor->setText ("3333");

    addAndMakeVisible (parameterslabel = new Label (T("number of parameters"),
                                               T("Number of parameters:")));
    parameterslabel->setFont (Font (15.0000f, Font::plain));
    parameterslabel->setJustificationType (Justification::centredLeft);
    parameterslabel->setEditable (false, false, false);
    parameterslabel->setColour (TextEditor::textColourId, Colours::black);
    parameterslabel->setColour (TextEditor::backgroundColourId, Colour (0x0));

    addAndMakeVisible (parametersEditor = new TextEditor (String::empty));
    parametersEditor->setMultiLine (false);
    parametersEditor->setReturnKeyStartsNewLine (false);
    parametersEditor->setReadOnly (false);
    parametersEditor->setScrollbarsShown (true);
    parametersEditor->setCaretVisible (true);
    parametersEditor->setPopupMenuEnabled (true);
    parametersEditor->setText ("1");

    addAndMakeVisible (okButton = new TextButton (String::empty));
    okButton->setButtonText (T("Accept"));
    okButton->addListener (this);
    okButton->setColour (TextButton::buttonColourId, Colour (0xffbfffbb));

    addAndMakeVisible (cancelButton = new TextButton (String::empty));
    cancelButton->setButtonText (T("Cancel"));
    cancelButton->addListener (this);
    cancelButton->setColour (TextButton::buttonColourId, Colour (0xffe67154));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (400, 137);

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]

	okClicked = false;
}

OscOutputConfigurator::~OscOutputConfigurator()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

	deleteAndZero (hostLabel);
	deleteAndZero (hostEditor);
    deleteAndZero (addressLabel);
    deleteAndZero (addressEditor);
    deleteAndZero (portlabel);
    deleteAndZero (portEditor);
    deleteAndZero (parameterslabel);
    deleteAndZero (parametersEditor);
    deleteAndZero (okButton);
    deleteAndZero (cancelButton);

    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void OscOutputConfigurator::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

	g.fillAll (Colours::lightgrey);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void OscOutputConfigurator::resized()
{
	hostLabel->setBounds(12, 8, 56, 24);
	hostEditor->setBounds(56, 8, 336, 24);
    addressLabel->setBounds (0, 40, 56, 24);
    addressEditor->setBounds (56, 40, 336, 24);
    portlabel->setBounds (16, 72, 40, 24);
    portEditor->setBounds (56, 72, 168, 24);
    parameterslabel->setBounds (8, 104, 160, 24);
    parametersEditor->setBounds (168, 104, 56, 24);
    okButton->setBounds (240, 72, 150, 24);
    cancelButton->setBounds (240, 104, 150, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void OscOutputConfigurator::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == okButton)
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

String OscOutputConfigurator::getHost()
{
	return hostEditor->getText();
}

String OscOutputConfigurator::getAddress()
{
	return addressEditor->getText();
}
	
int OscOutputConfigurator::getPort()
{
	return portEditor->getText().getIntValue();
}
	
int OscOutputConfigurator::getNumberOfParameters()
{
	return parametersEditor->getText().getIntValue();
}

void OscOutputConfigurator::setHost(String Host)
{
	hostEditor->setText(Host);
}

void OscOutputConfigurator::setAddress(String Address)
{
	addressEditor->setText(Address);
}

void OscOutputConfigurator::setPort(int Port)
{
	portEditor->setText(String(Port));
}

void OscOutputConfigurator::setNumberOfParameters(int NumberOfParameters)
{
	parametersEditor->setText(String(NumberOfParameters));
}