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
#include "CalibratorConfigurator.h"


class CalibratorNode  : public Node
{
public:
    //==============================================================================
    CalibratorNode ();
    ~CalibratorNode();

	void initialise(int NumberOfInputs, int NumberOfOutputs);

	virtual void process();

	void mouseDoubleClick(const MouseEvent &e);

	float getOutputMin(int index);
	float getOutputMax(int index);

	bool getMinClip(int index);
	bool getMaxClip(int index);

	CalibratorConfigurator* getConfigurator() {return configurator;};

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
	CalibratorConfigurator* configurator;
  
};
