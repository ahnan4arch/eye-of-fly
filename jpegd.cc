#include <fstream>
#include <cassert>

#include "jpegd.h"
#include "exception.h"


using namespace std;
using namespace vid;


namespace {
  void dump_header(std::shared_ptr<jpeg_decompress_struct> &);
}

void dump_header(std::shared_ptr<jpeg_decompress_struct> &j) 
{
  using namespace std;

  ofstream of("stat");
  if (of) {
	of << j->output_width;
	of << j->output_height;
	of << j->out_color_components;
  }

  of.close();
}

int 
JPEGDecoder::bind(uint8_t *src, size_t size, DecImgParams &r) 
{
  assert(src);
  int rc;
  jpeg_mem_src(jinfo.get(), src, size);

  rc = jpeg_read_header(jinfo.get(), TRUE);
  jpeg_start_decompress(jinfo.get());

  r.width      = jinfo->output_width;
  r.height     = jinfo->output_height;
  r.out_colors = jinfo->out_color_components;

  if (rc == 1) 
	start_set = true;

  return rc;
}

void 
JPEGDecoder::decompress(uint8_t *dst)
{
  if (!start_set)
	throw exception::init_error("jpeg_start_decompress (in bind) must be called before decompressing");

  while(jinfo->output_scanline < jinfo->output_height) {
	register uint8_t *where = dst + jinfo->output_width * jinfo->output_scanline;
	jpeg_read_scanlines(jinfo.get(), 
						&where,
						jinfo->output_height - jinfo->output_scanline);
  }

  jpeg_finish_decompress(jinfo.get());
  start_set = false;
}

void
JPEGDecoder::finish()
{
  jpeg_finish_decompress(jinfo.get());
}
