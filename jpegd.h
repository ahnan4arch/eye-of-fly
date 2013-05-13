#ifndef  _JPEGD_INC_
#define _JPEGD_INC_

#include <memory>
#include <vector>

#include <cstdint>


#include <cstdio>
#include <jpeglib.h>

#include "dim.h"

#define CACHE_SIZE 1 // 48

namespace vid {
  class JPEGDecoder;
}

class vid::JPEGDecoder {
public:
  JPEGDecoder() : 
	jinfo(new jpeg_decompress_struct),
	jerr (new jpeg_error_mgr        ),
	start_set(false),
	bytes_in_row(0)
  {
	jinfo->err = jpeg_std_error(jerr.get());
	jpeg_create_decompress(jinfo.get());

	//cache.reserve(CACHE_SIZE);
	//	std::fill_n(std::back_inserter(cache), CACHE_SIZE, nullptr);
  }

  ~JPEGDecoder() {
	jpeg_destroy(reinterpret_cast<j_common_ptr>(jinfo.get()));
  }

public:

  int  bind(uint8_t *src, size_t size, DecImgParams &);
  void decompress(uint8_t *to);
  void init_cache(uint8_t *dst);

  void finish();

private:
  std::shared_ptr<jpeg_decompress_struct> jinfo;
  std::shared_ptr<jpeg_error_mgr>         jerr;

  bool start_set;

  unsigned bytes_in_row;
  //  std::vector<uint8_t *> cache;
  uint8_t *cache[CACHE_SIZE];
};

#endif
