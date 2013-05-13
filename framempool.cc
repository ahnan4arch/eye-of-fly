#include <algorithm>
#include <iostream>
#include <sstream>

#include <unistd.h>

#include <Poco/Logger.h>

#include "config.h"
#include "framempool.h"

using Poco::MemoryPool;
using Poco::Logger;

using namespace vid;

#define CAST2UINT(v) static_cast<unsigned>(v)

#define dbg(m) std::cerr << __func__ << ": " << m << std::endl

std::shared_ptr<vid::FramePool> vid::FramePool::self;

namespace {
struct WrkBuffInitRecord {
  unsigned pg_num;
  unsigned max_alloc;
};
}

#define NELS(a)  (sizeof (a) / sizeof (a)[0])

// Note: pg_num should be in sorted order
static const WrkBuffInitRecord work_init_tbl[] =
  {
	{ 2,  MAX_CAMS_CONFIG },
	{ 4,  MAX_CAMS_CONFIG },
	{ 5,  MAX_CAMS_CONFIG },
	{ 8,  MAX_CAMS_CONFIG },
	{ 12, MAX_CAMS_CONFIG },
	{ 14, MAX_CAMS_CONFIG },
	{ 12, MAX_CAMS_CONFIG },
	{ 16, MAX_CAMS_CONFIG },
	{ 32, MAX_CAMS_CONFIG }
  };


bool vid::operator<(const WrkPoolSptr &lh, const WrkPoolSptr &rh)
{
  return lh->pg_num < rh->pg_num;
}



inline void *
WorkingPool::alloc()
{
  return pool->get();
}

inline void
WorkingPool::release(void *p)
{
  return pool->release(p);
}

FramePool::FramePool() : 
  sys_page_size(sysconf(_SC_PAGESIZE))
{
  register unsigned sz = static_cast<unsigned>(ImageType::IMAGE_TYPES_NUM);
  predefined.reserve(sz);

  init_page_size();

  //
  // Create result buffers for image
  //
  for(unsigned i = 0; i < sz; ++i)  {
	// dbg("get set: " << vid::get_size(i) << " max, prealloc: " <<  STREAM_ZBUFFER_SIZE * MAX_CAMS_CONFIG * vid::get_size(i)  );
	// MemoryPool *mp = new MemoryPool( vid::get_size(i),  
	// 							 STREAM_ZBUFFER_SIZE * MAX_CAMS_CONFIG,
	// 							 STREAM_ZBUFFER_SIZE * MAX_CAMS_CONFIG);
	// dbg("done");
	predefined.push_back(std::make_shared<MemoryPool>( vid::get_size(i),  
													   STREAM_ZBUFFER_SIZE * MAX_CAMS_CONFIG,
													   STREAM_ZBUFFER_SIZE * MAX_CAMS_CONFIG));
  }

  //
  // Create 'bootstrap' - page where initial data from camera will be written,
  // until we get full header and frame size (in bytes) from it
  //
  bootstrap = std::make_shared<MemoryPool>(sys_page_size);


  // 
  // Create working pools for comrpressed jpeg
  // 
  unsigned elms = NELS(work_init_tbl);
  wrk_pools.reserve(elms);

  for(auto &init : work_init_tbl) {
  	wrk_pools.push_back( std::make_shared<WorkingPool>(init.pg_num, sys_page_size) );
  }

}

const size_t
FramePool::bootstrap_size()  const
{
  return bootstrap->blockSize();
}

void *
FramePool::alloc_predef(ImageType type)
{
  //dbg("Allocating prefefined image buffer for type " << CAST2UINT(type) );
  return predefined[CAST2UINT(type)]->get();
}

void   
FramePool::release_predef(ImageType type, void *ptr)
{
  //dbg("Deallocating of type " << CAST2UINT(type) << " ptr => " << ptr );
  predefined[CAST2UINT(type)]->release(ptr);
}


FramePool::WorkingSlab
FramePool::alloc_wrk(size_t size)
{
  using namespace std;

  unsigned idx = wrk_pool_idx(size);
  return std::make_pair(idx, wrk_pools[idx]->alloc() );
}


void
FramePool::release_wrk(const WorkingSlab &victim)
{
  // Logger &d = Logger::get("dbg-connect");
  // poco_warning_f1(d, "release at pool with id => %u", victim.first);
  
  wrk_pools[victim.first]->release(victim.second);
}


void 
FramePool::init_page_size()
{
  if (sys_page_size == -1) {
	sys_page_size = DEFAULT_PAGE_SIZE;
  }
}

unsigned long
FramePool::pages_num(const size_t size) const
{
  const unsigned shift = __builtin_ctz(sys_page_size);

  unsigned long pages = size >> shift;
  if (size & (sys_page_size - 1)) {
	pages++;
  }

  return pages;
}

unsigned long
FramePool::wrk_pool_idx(const size_t size) const
{
  using namespace std;

  unsigned long pages = pages_num(size);

  // Logger &d = Logger::get("dbg-connect");
  // poco_warning_f1(d, " requesting page size => %lu", pages );

  WorkingPoolsVec::const_iterator it = find_if( begin(wrk_pools),
												end(wrk_pools), 
												[&pages] (const WrkPoolSptr &w) {
												  return pages <= w->pg_number(); 
												} );
  
  if (it == end(wrk_pools)) {
	ostringstream os;
	os << "number of " << pages << " pages not defined in pool" << endl;
	throw exception::out_of_range_error( os.str() );
  }
  
  
  //poco_warning_f2(d, " select index %lu, wrk. buff pg_num => %u", (unsigned long) distance( begin(wrk_pools), it), (*it)->pg_number() ) ;
  
  return distance( begin(wrk_pools), it);
}
