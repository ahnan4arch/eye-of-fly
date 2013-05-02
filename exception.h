#ifndef _EXCEPT_INC_
#define _EXCEPT_INC_

#include <stdexcept>
#include <exception>

namespace vid {
  namespace exception {
	class not_implemented;
	class logic_error;
	class init_error;
	class nomem_error;
	class out_of_range_error;
  }
}

class vid::exception::not_implemented : public std::runtime_error {
public:
  not_implemented(const std::string &what=" not defined ") : std::runtime_error(what) {}
};

class vid::exception::logic_error : public std::logic_error {
public:
  logic_error(const std::string &what=" logical error ") : std::logic_error(what) {}
};

class vid::exception::init_error : public std::runtime_error {
public:
  init_error(const std::string &what=" initializer error ") : std::runtime_error(what) {}
};

class vid::exception::nomem_error : public std::runtime_error {
public:
  nomem_error(const std::string &what=" not enough memory ") : std::runtime_error(what) {}
};

class vid::exception::out_of_range_error : public std::runtime_error {
public:
  out_of_range_error(const std::string &what=" not in range ") : std::runtime_error(what) {}
};


#endif
