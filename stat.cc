#include "stat.h"

using namespace vid;
using namespace std;

//
// Buggy atomic support in gcc <= 4.6
//
// Removed for now!
//

// void 
// StreamStat::update_jpeg_decode_time(unsigned long long dt)
// {
//   lock_guard<std::mutex> lock(stat_lock);
  
//   val_decode.store((dt + val_decode.load() / (cnt_decode + 1)));
//   cnt_decode++;
// }

// void 
// StreamStat::update_jpeg_retrieval_time(unsigned long long dt)
// {
//   lock_guard<std::mutex> lock(stat_lock);

//   val_net.store((dt + val_net.load() / (cnt_net + 1)));
//   cnt_net++;
// }

// void 
// StreamStat::update_gl_redraw_time(unsigned long long dt)
// {
//   lock_guard<std::mutex> lock(stat_lock);

//   val_gl.store((dt + val_gl.load() / (cnt_gl + 1)));
//   cnt_gl++;
// }

void 
StreamStat::update_jpeg_decode_time(unsigned long long dt)
{
  lock_guard<std::mutex> lock(stat_lock);

  val_decode = (dt + cnt_decode * val_decode) / (cnt_decode + 1);
  cnt_decode++;
}

void 
StreamStat::update_jpeg_retrieval_time(unsigned long long dt)
{
  lock_guard<std::mutex> lock(stat_lock);

  val_net = (dt + cnt_net * val_net) / (cnt_net + 1);
  cnt_net++;
}

void 
StreamStat::update_gl_redraw_time(unsigned long long dt)
{
  lock_guard<std::mutex> lock(stat_lock);

  val_gl = (dt + cnt_gl * val_gl) / (cnt_gl + 1);
  cnt_gl++;
}

void 
StreamStat::update_jpeg_size(unsigned long long dt)
{
  lock_guard<std::mutex> lock(stat_lock);

  val_jpeg_size = (dt + cnt_jpeg * val_jpeg_size) / (cnt_jpeg + 1);
  cnt_jpeg++;
}
