#ifndef _CAMCOM_INC
#define _CAMCOM_INC

#include "Poco/Format.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/SocketStream.h"

#include <memory>
#include <stdexcept>

#define NILL_SOCKET -1

using Poco::format;

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPSession;

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

  std::shared_ptr<HTTPClientSession> session;
public: // ctor/dtor section

  typedef Poco::UInt16 uint16;

  CamConnection(const std::string &url_, 
			   const std::string &cmd_, 
			   uint16 port=HTTPSession::HTTP_PORT) : 
   url(url_), 
   cmd(cmd_),
   conn_so(NILL_SOCKET),
   port(port) {}

public:

  int get_channel(const std::string &url, const std::string &cmd, uint16 port=HTTPSession::HTTP_PORT) {

	session         = std::make_shared(url, port);
	HTTPRequest       req(HTTPRequest::HTTP_GET, cmd, HTTPMessage::HTTP_1_1);
	HTTPResponse      resp;
	
	session->sendRequest(req);
	session->receiveResponse(resp);

	return conn_so = session->detachSocket().impl()->sockfd();
  }

  int connect() {
	if (url.empty()) {
	  throw std::logic_error("Can't connect: URL wasn't set");
	}

	if (cmd.empty()) {
	  throw std::logic_error("Can't connect: COMMAND wasn't set");
	}

	return get_channel(url, cmd, port);
  }

  void close() {
	if (session) {
	  session->reset();
	}
  }

public:

  void setCommand(const std::string &cmd_) { cmd = cmd_; }
  void setURL(const std::string &url_)     { url = url_; }

};


#endif
