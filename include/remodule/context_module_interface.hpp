#pragma once

#include <remodule/module_interface.hpp>

namespace remodule {
	template<class T>
	class REMODULE_EXPORT context_module_interface : public module_interface {
	public:
		virtual module_type get_type() const override final {
			return module_type::CONTEXT;
		}

		virtual void set_context(T* context) = 0;
		virtual ~context_module_interface() = default;
	};
}
