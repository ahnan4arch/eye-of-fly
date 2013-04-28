#ifndef _CAMCOM_INC
#define _CAMCOM_INC

#include <memory>
#include <stdexcept>
#include <cstdint>

#include "config.h"

#define NILL_SOCKET -1

namespace vid {
  class DeviceConnection;
  class CamConnection;
};


class vid::DeviceConnection {
public:
  virtual void connect(const std::string &url) = 0;
};

class vid::CamConnection {
  std::string url;
  std::string cmd;
  int conn_so;
  unsigned short port;

  //std::shared_ptr<HTTPClientSession> session;
  typedef std::shared_ptr<void> VoidConnSPtr;

  VoidConnSPtr vsp_so;
public: // ctor/dtor section

  CamConnection(const std::string &url_, 
			   const std::string &cmd_, 
			   unsigned port=DEFAULT_HTTP_PORT) : 
   url(url_), 
   cmd(cmd_),
   conn_so(NILL_SOCKET),
   port(port) {}

private:
  int get_channel(const std::string &url, 
				  const std::string &cmd, 
				  unsigned short port=DEFAULT_HTTP_PORT);
public:

  int connect() {
	if (url.empty()) {
	  throw std::logic_error("Can't connect: URL wasn't set");
	}

	if (cmd.empty()) {
	  throw std::logic_error("Can't connect: COMMAND wasn't set");
	}

	return get_channel(url, cmd, port);
  }

  VoidConnSPtr connect_ref() {
	int so = connect();
	return vsp_so;
  }

  void close() {
	// if (session) {
	//   session->reset();
	// }
  }

public:

  void setCommand(const std::string &cmd_) { cmd = cmd_; }
  void setURL(const std::string &url_)     { url = url_; }

  VoidConnSPtr get_so() const { return vsp_so; }

};


#endif
