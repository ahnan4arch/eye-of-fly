#include <memory>
#include <cstdlib>

#include <Poco/Logger.h>
#include <Poco/LogStream.h>
#include <Poco/ConsoleChannel.h>

#include <boost/exception/diagnostic_information.hpp>

#include "disp.h"


using Poco::Logger;
using Poco::LogStream;
using Poco::ConsoleChannel;

using namespace std;
using namespace vid;

class Runtime {
public:
  static int run(const std::string &url) {
	Dispatcher disp;
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
	disp.wait();

	return EXIT_SUCCESS;
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

  return Runtime::run(argv[1]);
}
