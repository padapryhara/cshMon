#include <string>
#include <iostream>
#include <numeric>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <zmqpp/zmqpp.hpp>

#include "hMonitor.h"
#include "signer.h"

using namespace std;
namespace pt = boost::property_tree;

hMonitor::protocol::statusType getRandStatus()
{
	return static_cast<hMonitor::protocol::statusType>(rand() % hMonitor::protocol::statusType::_last);
}

int main(int argc, char *argv[])
{
  bool isSubSystemsOn = true;
  hMonitor::protocol::statusType subSystemStatus1, subSystemStatus2;

  const string hbEndpoint = "tcp://*:" + to_string(hMonitor::heartbeatPort);
  const string cmdEndpoint = "tcp://*:" + to_string(hMonitor::commandPort);

  // initialize the 0MQ context
  zmqpp::context context;
  zmqpp::socket socketHb(context, zmqpp::socket_type::reply);
  zmqpp::socket socketCmd(context, zmqpp::socket_type::reply);

  // bind to the sockets
  socketHb.bind(hbEndpoint);
  socketCmd.bind(cmdEndpoint);

  Signer signer(hMonitor::seedKey, hMonitor::signBuffSz);
  while (true)
  {
    if (isSubSystemsOn)
    {
      subSystemStatus1 = getRandStatus();
      subSystemStatus2 = getRandStatus();
    }

    string hexMessage, messageText;
    if (socketHb.receive(hexMessage, true)) //heartbeat
    {
      if (!signer.checkSignature(hexMessage, messageText))
      {
        cout << "Error: sign check was failed" << endl;
        socketHb.send(string(""));
        continue;        
      }
      if (messageText.find(hMonitor::protocol::heartbeatMsg) == 0)
      {
        auto tmStamp = messageText.substr(hMonitor::protocol::heartbeatMsg.length());
        cout << "heartbeat timestamp: " << tmStamp << endl;

        pt::ptree tree;
        tree.put("AliveAt", tmStamp);
        tree.put("Status.subsystem1", subSystemStatus1);
        tree.put("Status.subsystem2", subSystemStatus2);
      
        stringstream ss;
        pt::json_parser::write_json(ss, tree);

        socketHb.send(ss.str());
      }
    }
    
    if (socketCmd.receive(hexMessage, true)) //command
    {
      if (!signer.checkSignature(hexMessage, messageText))
      {
        cout << "Error: sign check was failed" << endl;
        socketHb.send(string(""));
        continue;        
      }
      if (messageText.find(hMonitor::protocol::stopMsg) == 0)
      {
        auto tmStamp = messageText.substr(hMonitor::protocol::stopMsg.length());
        cout << "stop timestamp: " << tmStamp << endl;

        isSubSystemsOn = false;
        subSystemStatus1 = subSystemStatus2 = hMonitor::protocol::statusType::OFF;

        socketCmd.send(string("")); //TODO: ACK
      }
    }
  }
}