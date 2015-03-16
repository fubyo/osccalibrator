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
#include "../oscpack/osc/OscOutboundPacketStream.h"
#include "../oscpack/ip/UdpSocket.h"
#include "../oscpack/osc/OscReceivedElements.h"
#include "../oscpack/osc/OscPacketListener.h"

#include "..\juce\juce_amalgamated.h"
#include "Node.h"

#include <vector>
using namespace std;

class MainComponent;

struct ReceiverRegistration
{
	String address;
	int numberOfParameters;
	int port;
	Node* receiverNode;
	bool remoteAdding;
	bool remoteClearing;
};

struct TransmitSocket
{
	UdpTransmitSocket* udpsocket;
	int port;
	String host;
};

class SocketThread: public Thread, public osc::OscPacketListener
{
public:
	SocketThread(vector<ReceiverRegistration> *Receivers, int Port, MainComponent* theMainComponent);
	~SocketThread();

	int getPort();

	void run();
	void Break();

private:
	vector<ReceiverRegistration> *receivers;
	int port;
	UdpListeningReceiveSocket* s;

	MainComponent* mainComponent;

protected:
	virtual void ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint);
};



class OscManager : public Thread
{
public:
    //==============================================================================
    OscManager ();
    ~OscManager();

	void registerReceiver(String Address, int NumberOfParameters, int Port, Node* ReceiverNode);
	void unregisterReceiver(Node* ReceiverNode);

	void setMainComponent(MainComponent *theMainComponent);
	void sendOSC(String Host, int Port, String Address, vector<float> Parameters);
	void deleteTransmitSocket(String Host, int Port);

	void setRemoteAdding(bool State, String Address, int Port);
	void setRemoteClearing(bool State, String Address, int Port);

	void run();
	void stop();

	//==============================================================================
    juce_UseDebuggingNewOperator

private:
	vector<ReceiverRegistration> receivers;
	vector<SocketThread*> sockets;
	vector<TransmitSocket> transmitSockets;
	char buffer[1024];

	MainComponent *mainComponent;

	CriticalSection cs;
};