#ifndef  _JPEGD_INC_
#define _JPEGD_INC_

#include <memory>
#include <cstdint>

#include <cstdio>
#include <jpeglib.h>

#include "dim.h"

namespace vid {
  class JPEGDecoder;
}

class vid::JPEGDecoder {
public:
  JPEGDecoder() : 
	jinfo(new jpeg_decompress_struct),
	jerr (new jpeg_error_mgr        ),
	start_set(false)
  {
	jinfo->err = jpeg_std_error(jerr.get());
	jpeg_create_decompress(jinfo.get());
  }

  ~JPEGDecoder() {
	jpeg_destroy(reinterpret_cast<j_common_ptr>(jinfo.get()));
  }

public:

  int  bind(uint8_t *src, size_t size, DecImgParams &);
  void decompress(uint8_t *to);

  void finish();

private:
  std::shared_ptr<jpeg_decompress_struct> jinfo;
  std::shared_ptr<jpeg_error_mgr>         jerr;

  bool start_set;
};

#endif
