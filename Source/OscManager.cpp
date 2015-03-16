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


#include "OscManager.h"
#include "MainComponent.h"

OscManager::OscManager() : Thread("OscManager")
{
	 mainComponent=0;
	 Pool::Instance()->reg("OscManager", this);
}

OscManager::~OscManager()
{
	
}

void OscManager::setMainComponent(MainComponent *theMainComponent)
{
	mainComponent = theMainComponent;
}

void OscManager::registerReceiver(String Address, int NumberOfParameters, int Port, Node* ReceiverNode)
{
	cs.enter();

	ReceiverRegistration receiver;
	receiver.address=Address;
	receiver.numberOfParameters=NumberOfParameters;
	receiver.port=Port;
	receiver.receiverNode=ReceiverNode;
	receiver.remoteAdding=false;
	receiver.remoteClearing=false;

	receivers.push_back(receiver);
	
	cs.exit();
}

void OscManager::unregisterReceiver(Node* ReceiverNode)
{
	cs.enter();

	for (int i=0; i<receivers.size(); i++)
		if (receivers[i].receiverNode==ReceiverNode)
		{
			receivers.erase(receivers.begin()+i);
			cs.exit();
			return;
		}

	cs.exit();
}

void OscManager::run()
{
	while (!threadShouldExit())
	{
		//Update Sockets: Check if new sockets are needed
		for (unsigned int i=0; i<receivers.size(); i++)
		{
			bool found=false;

			for (unsigned int j=0; j<sockets.size(); j++)
			{
				if (sockets[j]->getPort()==receivers[i].port)
					found=true;
			}

			if (!found)
			{
				SocketThread* st = new SocketThread(&receivers, receivers[i].port, mainComponent);
				if (st)
				{
					sockets.push_back(st);
					st->startThread();
				}
			}
		}

		//Update Sockets: Remove not needed sockets
		cs.enter();

		int j=0;
		while (j<sockets.size())
		{
			bool found=false;

			for (unsigned int i=0; i<receivers.size(); i++)
			{
				if (sockets[j]->getPort()==receivers[i].port)
					found=true;
			}

			if (!found)
			{
				sockets[j]->Break();
				sockets[j]->stopThread(100);
				delete sockets[j];
				sockets.erase(sockets.begin()+j);
			}
			else
				j++;
		}

		cs.exit();

		sleep(10);
	}
}

void OscManager::stop()
{
	cs.enter();

	for (unsigned int i=0; i<sockets.size(); i++)
	{
		sockets[i]->Break();
		sockets[i]->stopThread(100);
		delete sockets[i];
	}
	sockets.clear();

	cs.exit();

	stopThread(500);
}

SocketThread::SocketThread(vector<ReceiverRegistration> *Receivers, int Port, MainComponent* theMainComponent) : Thread("SocketThread")
{
	port = Port;
	receivers=Receivers;
	s = new UdpListeningReceiveSocket(IpEndpointName(IpEndpointName::ANY_ADDRESS, Port), this);

	mainComponent=theMainComponent;
}
	
SocketThread::~SocketThread()
{
	delete s;
}

int SocketThread::getPort()
{
	return port;
}

void SocketThread::run()
{
	if (!threadShouldExit())
	{
		s->Run();
		sleep(5);
	}
	
}

void SocketThread::Break()
{
	s->AsynchronousBreak();
}

void SocketThread::ProcessMessage(const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint)
{
	bool haveToProcess = false;
	bool haveToAddCalibrationPoint = false;
	bool haveToClearCalibration = false;

	for (unsigned int i=0; i<receivers->size(); i++)
	{
		String recAddress = (*receivers)[i].address.trim();
		String mesAddress = String(m.AddressPattern()).trim();

		if (mesAddress.matchesWildcard(recAddress, false))
		{
			vector<float> arguments;
			osc::ReceivedMessage::const_iterator arg = m.ArgumentsBegin();
			for (long i=0; i<m.ArgumentCount(); i++)
			{
				arguments.push_back(arg->AsFloatUnchecked());
				arg++;
			}

			for (unsigned int j=0; j<(*receivers)[i].receiverNode->getNumberOfOutputs(); j++)
			{
				if (j<m.ArgumentCount())
					(*receivers)[i].receiverNode->setOutputValue(j, arguments[j]);
				else
					(*receivers)[i].receiverNode->setOutputValue(j, 0);
			}

			if ( (*receivers)[i].remoteAdding && mainComponent->activeConfigurator)
				haveToAddCalibrationPoint = true;
				

			if ( (*receivers)[i].remoteClearing && mainComponent->activeConfigurator)
				haveToClearCalibration = true;
				

			haveToProcess = true;
		}
	}

	if (mainComponent && haveToProcess)
		mainComponent->process();

	if (haveToAddCalibrationPoint || haveToClearCalibration)
	{
		const MessageManagerLock mmLock;

		if (mainComponent && haveToAddCalibrationPoint)
			mainComponent->activeConfigurator->buttonClicked(mainComponent->activeConfigurator->addButton);

		if (mainComponent && haveToClearCalibration)
			mainComponent->activeConfigurator->buttonClicked(mainComponent->activeConfigurator->clearButton);
	}

}

void OscManager::sendOSC(String Host, int Port, String Address, vector<float> Parameters)
{
	int found = -1;
	for (int i=0; i<transmitSockets.size(); i++)
	{
		if (transmitSockets[i].host.compare(Host) == 0 && transmitSockets[i].port==Port)
		{
			found = i;
		}
	}

	if (found == -1)
	{
		TransmitSocket ts;
		ts.host = Host;
		ts.port = Port;
		ts.udpsocket = new UdpTransmitSocket(IpEndpointName( Host.toCString(), Port ));

		transmitSockets.push_back(ts);
		found = transmitSockets.size()-1;
	}

	UdpTransmitSocket* transmitSocket = transmitSockets[found].udpsocket;

    osc::OutboundPacketStream p(buffer, 1024);
    
	p << osc::BeginMessage( Address.toCString() );

	for (unsigned int i=0; i < Parameters.size(); i++)
	{
		p << Parameters[i];
	}

	p << osc::EndMessage;
    
    transmitSocket->Send( p.Data(), p.Size() );
}

void OscManager::deleteTransmitSocket(String Host, int Port)
{
	for (int i=0; i<transmitSockets.size(); i++)
	{
		if (transmitSockets[i].host.compare(Host) == 0 && transmitSockets[i].port==Port)
		{
			delete transmitSockets[i].udpsocket;
			transmitSockets.erase(transmitSockets.begin()+i);
			return;
		}
	}
}


void OscManager::setRemoteAdding(bool State, String Address, int Port)
{
	for (unsigned int i=0; i<receivers.size(); i++)
	{
		if (State)
		{
			if (receivers[i].address==Address && receivers[i].port==Port)
			{
				receivers[i].remoteAdding=true;
				return;
			}
		}
		else
			if (receivers[i].remoteAdding)
				receivers[i].remoteAdding=false;
	}
}

void OscManager::setRemoteClearing(bool State, String Address, int Port)
{
	for (unsigned int i=0; i<receivers.size(); i++)
	{
		if (State)
			{
				if (receivers[i].address==Address && receivers[i].port==Port)
				{
					receivers[i].remoteClearing=true;
					return;
				}
			}
			else
				if (receivers[i].remoteClearing)
					receivers[i].remoteClearing=false;
	}
}
