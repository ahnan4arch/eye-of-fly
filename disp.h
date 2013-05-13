#ifndef _DISP_INC_
#define _DISP_INC_

#include <map>
#include <memory>
#include <thread>
#include <tuple>

#include <boost/asio.hpp>

#include "streamer.h"

namespace vid {
  namespace ba = boost::asio;
  class Dispatcher;

  //  enum class StreamStatus {
  // 	Active,
  // 	Disabled
  // };
}

#include "cam.h"
#include "view.h"

class vid::Dispatcher {

  typedef std::map<std::string, CameraSPtr   >    CamerasMap;
  typedef std::shared_ptr<std::thread> ThreadSPtr;

public:
  
  void run(bool should_join=false);

  void wait();

  /// io_service to provide to streamers upon construction
  ba::io_service &service() {
	return io;
  }

  ///
  /// @param view  view to bind with streeam
  /// @param cname camera name
  /// @param sname stream name
  /// 
  void bind( IView::pointer view, const std::string &cname, const std::string &sname);

  void add_camera(const std::string &, CameraSPtr);

private:
  ba::io_service io;

  CamerasMap cams;
  std::vector<ThreadSPtr> thrs;
};

#endif
