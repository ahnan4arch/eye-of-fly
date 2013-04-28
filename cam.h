#ifndef _CAM_INC_
#define _CAM_INC_

#include <list>
#include <tuple>
#include <memory>

#include "dim.h"

// forward declaration, note: 'includes' can follow it
namespace vid {

  class Camera;
  class NVCamera;

  typedef std::shared_ptr<Camera>   CameraSPtr;

  /// Mostly for use inside straemer
  typedef std::weak_ptr<Camera> CameraWeakRef;
}

#include "streamer.h"

namespace vid {
  namespace ba = boost::asio;

  //  typedef std::list<Frame>          FramesList;
  typedef std::map<std::string, StreamerSPtr> StreamsMap;
}

#include "disp.h"


///
/// @class Camera
///
/// @brief Conrete JES camera class (for now, later will be transformed into hierarchy of models)
///
class vid::Camera :
  public std::enable_shared_from_this<Camera> {

protected:
  
  Camera(const std::string &url, 
		 const std::string &idname, 
		 const std::string &desc="",
		 unsigned short port=DEFAULT_HTTP_PORT) : 
	url(url),
	port(port),
	idname(idname),
	dsc(desc) {}

public:

  void add_streamer(vid::Dispatcher &, 
					const std::string &name, 
					const VideoMode &vmode, 
					int id=0);

  void del_streamer(const std::string &name);
  

  /// Should check for pointer validity
  std::pair<bool,StreamerSPtr> stream(const std::string &name);

  virtual std::string get_url() const {
  	return url;
  }

  virtual std::string desc() const {
  	return dsc;
  }


  std::string id_name() {
	return idname;
  }

  void id_name(const std::string &new_name) {
	idname = new_name;
  }

protected:
  /// @brief make a string representation of command for this specific model
  virtual std::string prepare_get_cmd(const VideoMode &, const std::string &name, int id) = 0;

private:
  //std::vector<StreamerSPtr> streams;
  //std::map<int, NamedStreamerSPtr> streams;
  StreamsMap streams;

  std::string url;    
  unsigned short port;

  std::string idname;
  std::string dsc;

  int id; ///< Not used for now. For database reference.

  const static std::string get_cmd_args_format;
};

class vid::NVCamera : public vid::Camera {
protected:

  NVCamera(const std::string &url, 
		   const std::string &idname, 
		   const std::string &desc="",
		   unsigned short port=DEFAULT_HTTP_PORT) : 
	Camera(url, idname, desc, port) {}

protected:
  virtual std::string prepare_get_cmd(const VideoMode &, const std::string &name, int id);
private:
  const static std::string get_cmd_args_format;

public:
  static CameraSPtr make(const std::string &url, 
				  const std::string &idname, 
				  const std::string &desc="",
				  unsigned short port=DEFAULT_HTTP_PORT) {
	return CameraSPtr(new NVCamera(url, idname, desc, port));
  }
};


namespace vid {
  bool operator<(const vid::CameraSPtr &lhr, const vid::CameraSPtr &rhr);
}

#endif
