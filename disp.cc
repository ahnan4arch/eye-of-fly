#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <thread>

#include <Poco/Logger.h>

#include "disp.h"


using namespace vid;

using Poco::Logger;

void
Dispatcher::run(bool should_join) 
{
  using namespace std;
  typedef size_t (ba::io_service::*IORunMethodPtr)();
  //  using namespace boost::asio;

  Logger &dlog = Logger::get("dbg-connect");

  int ncores = thread::hardware_concurrency();
  poco_warning_f1( dlog, "Number of cores (by lib) => %d", ncores);

  if (ncores <= 1) {
	ncores = DEFAULT_IO_THREADS_NUMBER; // 2 threads default value
  }

  thrs.reserve(ncores);
  
  generate_n(back_inserter(thrs), ncores,
  		   [&, this] () 
  		   { 
			 poco_warning(dlog, "Starting thread...");
  			 return ThreadSPtr( new thread(std::bind( static_cast<IORunMethodPtr>(&ba::io_service::run), &this->io) ) ); 
  			   } 
  		   );


  


  if (should_join)
	wait();

}

void
Dispatcher::wait()
{
  std::for_each(begin(thrs), end(thrs), 
			[&] (ThreadSPtr t) { t->join(); } );  
	
}
