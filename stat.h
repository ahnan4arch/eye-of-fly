#ifndef _STAT_INC_
#define _STAT_INC_

#include <atomic>
#include <thread>
#include <memory>

namespace vid {
  class StreamStat;
}

using namespace std;

class vid::StreamStat {
public:
  typedef std::shared_ptr<StreamStat> pointer;
  
  static StreamStat::pointer instance() {

	static StreamStat::pointer self;
	if (!self) {
	  self.reset(new StreamStat);
	}

	return self;
  }

public:

  void update_jpeg_decode_time(unsigned long long dt);
  void update_jpeg_retrieval_time(unsigned long long dt);
  void update_gl_redraw_time(unsigned long long dt);
  void update_jpeg_size(unsigned long long dt);

  double avg_decode() const { return val_decode;    }
  double avg_net()    const { return val_net;       }
  double avg_gl()     const { return val_gl;        }
  double avg_jsize()  const { return val_jpeg_size; }
  

private:
  std::mutex stat_lock;

  // Event counters
  unsigned long long cnt_decode;
  unsigned long long cnt_net;
  unsigned long long cnt_gl;
  unsigned long long cnt_jpeg;

  // Values: commulative average
  // Not implemented yet in gcc <= 4.6
  // Removed for now
  // std::atomic<double> val_decode;
  // std::atomic<double> val_net;  
  // std::atomic<double> val_gl;

  double val_decode;
  double val_net;  
  double val_gl;

  double val_jpeg_size;

private:
  StreamStat() :
	cnt_decode(0),
	cnt_net(0),
	cnt_gl(0),
	val_decode(0.),
	val_net(0.),
	val_gl(0.),
	val_jpeg_size(0)
  {}

  StreamStat(const StreamStat &) = delete;
};

#endif
