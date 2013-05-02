#include "camcon.h"

#include "Poco/Logger.h"
#include "Poco/LogStream.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/StreamSocket.h"

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::Net::HTTPSession;
using Poco::Net::StreamSocket;

using Poco::Logger;
using Poco::LogStream;

using namespace vid;

int
CamConnection::get_channel(const std::string &url, const std::string &cmd, unsigned short port) 
{
  using namespace std;

  Logger &logger = Logger::get("dbg-connect");
  LogStream log(logger);

  log << "Creating session..." << endl;
  HTTPClientSession session(url, port);

  log << "Creating request..." << endl;
  HTTPRequest       req(HTTPRequest::HTTP_GET, cmd, HTTPMessage::HTTP_1_1);
  HTTPResponse      resp;	

  log << "Sending request (actually connecting)..." << endl;	
  session.sendRequest(req);

  log << "Getting response..." << endl;
  session.receiveResponse(resp);

  log << "Return native socket to callee" << endl;

  vsp_so.reset( new StreamSocket( session.detachSocket() ) );
  return conn_so = static_cast<StreamSocket*>( vsp_so.get() )->impl()->sockfd();
}
