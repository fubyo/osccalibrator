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
#include "OscOutputConfigurator.h"
#include "OscManager.h"
#include "Pool.h"

class OscOutputNode  : public Node
{
public:
    //==============================================================================
    OscOutputNode ();
    ~OscOutputNode();

	//void setOscManager(OscManager* theOscManager) {oscManager=theOscManager;};

	void mouseDoubleClick(const MouseEvent &e);

	String getAddress() {return address;};
	String getHost() {return host;};
	int getPort() {return port;};
	void setAddress(String Address) {address=Address;};
	void setHost(String Host);
	void setPort(int Port);

	virtual void process();

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
	OscManager* oscManager;
	String host;
	int port;
	String address;

//	UdpTransmitSocket* transmitSocket;
//	char buffer[512];
};
