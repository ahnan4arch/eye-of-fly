#include <iostream>
#include <memory>

#include <Poco/Logger.h>
#include <Poco/LogStream.h>
#include <Poco/ConsoleChannel.h>

#include <boost/exception/diagnostic_information.hpp>

#include "disp.h"
#include "view.h"


using Poco::Logger;
using Poco::LogStream;
using Poco::ConsoleChannel;

using namespace std;
using namespace vid;

namespace vid {
  class Runtime;
}

class vid::Runtime {
public:

  static int run(int argc, char *argv[]) {

	std::string url( (argc >= 2) ? argv[1] : "192.168.13.14");

	Dispatcher disp;
	//boost::asio::io_service::work work(disp.service());

	CameraSPtr cam1 = NVCamera::make(url, "test1"); 

	try {
	  cam1->add_streamer(disp, "ch1", std::make_tuple(ImageType::SIZE640x480c, Fps::FPS7) );
	} catch(std::exception &e) {
	  LogStream ls(Logger::get("dbg-connect"));
	  ls << "Failed to add streamer. exception => " << e.what() << endl;
	  ls << "  info => " << boost::diagnostic_information(e) << endl;
	  throw;
	}

	disp.run();
	LogStream ls(Logger::get("dbg-connect"));
	ls << "Getting data..."  << endl;

	//	StreamerSPtr stream;
	int gui_ret = MainView::run_test( make_shared<StubFrameReceiver>(), cam1->stream("ch1").second , argc, argv);
	ls << "Gui running..."  << endl;

	cam1->del_streamer("ch1");

	//disp.service().stop();
	disp.wait();

	return gui_ret;
  }
  
  static int usage() {
	using namespace std;
	
	cerr << "run with parameters: <url>\n";
	
	return EXIT_FAILURE;
  }

  static void init() {
	Logger::get("dbg-connect").setChannel(new ConsoleChannel );
  }
};


int
main(int argc, char *argv[] )
{
  using namespace std;

  if (argc < 2) {
	return Runtime::usage();
  }

  Runtime::init();
  return Runtime::run( argc, argv);
}
