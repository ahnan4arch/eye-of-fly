#include <iostream>

#include "config.h"
#include "framempool.h"

using Poco::MemoryPool;
using namespace vid;

#define CAST2UINT(v) static_cast<unsigned>(v)

#define dbg(m) std::cerr << m << std::endl

std::shared_ptr<vid::FramePool> vid::FramePool::self;

FramePool::FramePool() 
{
  register unsigned sz = static_cast<unsigned>(ImageType::IMAGE_TYPES_NUM);
  predefined.reserve(sz);

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

  bootstrap = std::make_shared<MemoryPool>(SYS_PAGE_SIZE);
}

const size_t
FramePool::bootstrap_size()  const
{
  return bootstrap->blockSize();
}

void *
FramePool::alloc_predef(ImageType type)
{
  return predefined[CAST2UINT(type)]->get();
}

void   
FramePool::release_predef(ImageType type, void *ptr)
{
  predefined[CAST2UINT(type)]->release(ptr);
}
