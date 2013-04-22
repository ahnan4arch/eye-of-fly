#include <algorithm>
#include <functional>
#include <memory>
#include <thread>

#include "disp.h"

using namespace vid;

void
Dispatcher::run(bool should_join) 
{
  using namespace std;
  typedef size_t (ba::io_service::*IORunMethodPtr)();
  //  using namespace boost::asio;

  int ncores = thread::hardware_concurrency();
  if (ncores <= 1) {
	ncores = DEFAULT_IO_THREADS_NUMBER; // 2 threads default value
  }

  thrs.reserve(ncores);
  
  generate(begin(thrs), end(thrs), 
		   [&, this] () 
		   { 
			 return ThreadSPtr( new thread(std::bind( static_cast<IORunMethodPtr>(&ba::io_service::run), &io) ) ); 
			   } 
		   );

  if (should_join) {
	wait();
  }

}

void
Dispatcher::wait()
{
  std::for_each(begin(thrs), end(thrs), 
			[&] (ThreadSPtr t) { t->join(); } );  
	
}
