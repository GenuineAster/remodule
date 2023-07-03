#include <remodule/module_interface.hpp>

namespace remodule {
	module_type module_interface::get_type() const {
		return module_type::SIMPLE;
	}
}
