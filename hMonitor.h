#pragma once

namespace hMonitor
{
  const static unsigned int heartbeatPort = 12277;
  const static unsigned int commandPort = 12278;
  const static unsigned int webServerPort = 9080;
  const static unsigned int signBuffSz = 256;
  
  const static std::string seedKey = "A special seed sequence for hMon"; //seed for public key generation (instead of exchange)

  struct protocol
  {
  	const static std::string heartbeatMsg;
  	const static std::string stopMsg;

    typedef enum
    {
	  OK,
	  OFF, //TODO: should I set this item to zero(false)?
	  WARNING,
	  ERROR,
	  CRITICAL,
	  //TODO:add new items here

	  _last
    } statusType;
 };

  const std::string protocol::heartbeatMsg = "areYouAlive+";
  const std::string protocol::stopMsg = "stop+";
}