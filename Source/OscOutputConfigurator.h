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

class OscOutputConfigurator  : public Component,
                              public ButtonListener
{
public:
    //==============================================================================
    OscOutputConfigurator ();
    ~OscOutputConfigurator();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void buttonClicked (Button* buttonThatWasClicked);

	String getHost();
	String getAddress();
	int getPort();
	int getNumberOfParameters();
	bool okClicked;

	void setHost(String Host);
	void setAddress(String Address);
	void setPort(int Port);
	void setNumberOfParameters(int NumberOfParameters);

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
	Label* hostLabel;
	TextEditor* hostEditor;
    Label* addressLabel;
    TextEditor* addressEditor;
    Label* portlabel;
    TextEditor* portEditor;
    Label* parameterslabel;
    TextEditor* parametersEditor;
    TextButton* okButton;
    TextButton* cancelButton;

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    OscOutputConfigurator (const OscOutputConfigurator&);
    const OscOutputConfigurator& operator= (const OscOutputConfigurator&);
};