#include <cstdlib>


#include "disp.h"

using namespace vid;


class Runtime {
public:
  static int run(const std::string &url) {
	Dispatcher disp;
	NVCamera cam1(url, "test1");

	cam1.add_streamer(disp, "ch1", std::make_tuple(ImageType::SIZE640x480c, Fps::FPS7) );
	
	disp.run();
	disp.wait();

	return EXIT_SUCCESS;
  }
};

int
usage()
{
  using namespace std;
  
  cerr << "run with parameters: <url>\n";

  return EXIT_FAILURE;
}

int
main(int argc, char *argv[] )
{
  using namespace std;

  if (argc < 2) {
	return usage();
  }

  return Runtime::run(argv[1]);
}
