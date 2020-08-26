#include <string>
#include <future>
#include <iostream>
#include <numeric>

#include <zmqpp/zmqpp.hpp>
#include "hMonitor.h"

using namespace std;


//command processor
static void waitForCmd(zmqpp::socket& socket)
{    
  string line;
  while (true)
  {
    cin >> line;
    if (line == "s")
    {
      auto message = hMonitor::protocol::stopMsg + to_string(time(nullptr));
      socket.send(message);

      string buffer;
      socket.receive(buffer);
    }    	
  }
}


int main(int argc, char *argv[])
{	
  if (argc != 2)
  {
  	cout << "Usage: " << string(argv[0]) << " <client_ip_address>"  << endl;
  	return -1;
  }	

  const string hbEndpoint = "tcp://" + string(argv[1]) + ":" + to_string(hMonitor::heartbeatPort);
  const string cmdEndpoint = "tcp://" + string(argv[1]) + ":" + to_string(hMonitor::commandPort);

  // initialize the 0MQ context
  zmqpp::context context;
  zmqpp::socket socketHb(context, zmqpp::socket_type::req);
  zmqpp::socket socketCmd(context, zmqpp::socket_type::req);

  // open the connection
  cout << "Connecting…" << endl;
  socketHb.connect(hbEndpoint);
  socketCmd.connect(cmdEndpoint);
  cout << "Type `s`+ Enter to stop all subsystems…" << endl << flush;
  std::thread sendCmdThread(waitForCmd, std::ref(socketCmd));
  while (true)
  {
    // send a message
    cout << "Sending heartbeat msg… ";    
    string msg = hMonitor::protocol::heartbeatMsg + to_string(time(nullptr));
    socketHb.send(msg);

    string buffer;
    socketHb.receive(buffer);
    
    cout << "Response: " << endl << buffer << endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}