#ifndef _DISP_INC_
#define _DISP_INC_

#include <map>
#include <memory>
#include <thread>
#include <tuple>

#include <boost/asio.hpp>


namespace vid {
  namespace ba = boost::asio;
  class Dispatcher;

  enum class StreamStatus {
	Active,
	Disabled
  };
}

#include "cam.h"
#include "streamer.h"


class vid::Dispatcher {

  typedef std::tuple<StreamerSPtr, enum StreamStatus> StreamRecord;
  typedef std::map<CameraSPtr, StreamRecord > StreamersMap;

  typedef std::list<StreamRecord> PollList;
  typedef std::map<StreamerSPtr, PollList::iterator> PollItersCache;

  typedef std::shared_ptr<std::thread> ThreadSPtr;

public:
  
  void run(bool should_join=false);

  void wait();

  /// io_service to provide to streamers upon construction
  ba::io_service &service() {
	return io;
  }

private:
  ba::io_service io;

  PollList       active;
  PollItersCache icache;

  // Seems that map is ok as we will make formatted output in some order (by key)
  StreamersMap streamers;

  std::vector<ThreadSPtr> thrs;
};

#endif
