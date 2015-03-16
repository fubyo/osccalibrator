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
#include "OscInputConfigurator.h"
#include "OscManager.h"

class OscInputNode  : public Node
{
public:
    //==============================================================================
    OscInputNode ();
    ~OscInputNode();

	void setOscManager(OscManager* theOscManager) {oscManager=theOscManager;};

	void mouseDoubleClick(const MouseEvent &e);

	String getAddress() {return address;};
	int getPort() {return port;};
	void setAddress(String Address) {address=Address;};
	void setPort(int Port) {port=Port;};

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
	OscManager* oscManager;
	String address;
	int port;
};
