#include <mutex> 
#include <string>
#include <future>
#include <iostream>
#include <numeric>

#include <zmqpp/zmqpp.hpp>
#include <pistache/endpoint.h>
#include "hMonitor.h"

using namespace std;

//web-server code
using namespace Pistache;

struct AtomicStatus
{
    string get() const
    {
      const std::lock_guard<std::mutex> guard(statusLock);
      return string(status);
    }

    void set(const std::string& value)
    {
      const std::lock_guard<std::mutex> guard(statusLock);
      status = value;
    }

  private:  
    mutable std::mutex statusLock;
    std::string status;
};

class SubSystemStatusHandler : public Http::Handler
{
  public:
    explicit SubSystemStatusHandler(AtomicStatus& status)
      : subSystemStatus(status)
    { }

    HTTP_PROTOTYPE(SubSystemStatusHandler)

    void onRequest(const Http::Request& request, Http::ResponseWriter response) override
    {
      UNUSED(request);
      response.send(Http::Code::Ok, subSystemStatus.get(), MIME(Application, Json));
    }

  private:
    AtomicStatus& subSystemStatus;
};

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
      socket.receive(buffer); //TODO: ACK with error code
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

  //web-server init
  Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(hMonitor::webServerPort));
  auto opts = Http::Endpoint::options().threads(1);
  Http::Endpoint server(addr);
  
  server.init(opts);
  AtomicStatus status;
  auto webHandler = Http::make_handler<SubSystemStatusHandler>(status);
  server.setHandler(webHandler);
  server.serveThreaded();

  //endpoints
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
    status.set(buffer);
    
    cout << "Response: " << endl << buffer << endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  //TODO: server.shutdown();
}