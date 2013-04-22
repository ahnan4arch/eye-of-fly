#include <boost/format.hpp>

#include "cam.h"
#include "camcon.h"
#include "framempool.h"

using namespace vid;
using namespace std;

const std::string vid::Camera::get_cmd_args_format = "none";

const std::string vid::NVCamera::get_cmd_args_format = 
  "/cgi-bin/fwstream.cgi?"												\
  "FwModId=0&AppKey=0x0450f000&PortId=%d&PauseTime=5&FwCgiVer=0x0001";


// std::string 
// Camera::prepare_cmd(const VideoMode &, const std::string &name, int id) 
// {
//   return "";
// }

std::pair<bool,StreamerSPtr>
Camera::stream(const std::string &name) 
{
  StreamsMap::iterator r = streams.find(name);

  if (r == streams.end()) {
	return make_pair(false,StreamerSPtr());
  }

  return make_pair(true, r->second);
}


void 
Camera::add_streamer(vid::Dispatcher &d, const std::string &name, const VideoMode &vmode, int id)
{

  //
  // 0. Get global memory pool
  //
  FramePool::FramePoolSPtr ppool = FramePool::global_pool();
  if (!ppool) {
	throw vid::exception::init_error("can't get global memory pool");
  }

  //
  // 1. Try to connect
  //
  CamConnection c(url, 
					 prepare_get_cmd(vmode, name, id), 
					 port);
  int sock   =  c.connect();

  if (NILL_SOCKET == sock) {
	throw vid::exception::init_error("can't make a connection to device");
  }

  //
  // 2. Create streamer object
  //
  JESStreamer::pointer s = JESStreamer::make_streamer(shared_from_this(), 
													   d.service(), *ppool, sock );
  if (!s) { //report an error
	close(sock);
	throw vid::exception::init_error("failed to create streamer");
  }

  //
  // 3. Store streamer object in local set
  //
  StreamsMap::iterator it = streams.find(name);
  if (it != streams.end()) { // already exists, close it
	it->second->close();
  }

  streams.insert( std::make_pair(name, s) );
}


void
Camera::del_streamer(const std::string &name)
{
  StreamsMap::iterator it = streams.find(name);
  if (it != streams.end()) {
	it->second->close();
	streams.erase(it);
  } // else: do nothing, we don't have this member
}

bool operator<(const vid::CameraSPtr &lhr, const vid::CameraSPtr &rhr) 
{
  return lhr->id_name() < rhr->id_name();
}

///
/// NVCamera methods
///
std::string 
NVCamera::prepare_get_cmd(const VideoMode &, const std::string &name, int id)
{  
  return str(boost::format(get_cmd_args_format) % id);
}
