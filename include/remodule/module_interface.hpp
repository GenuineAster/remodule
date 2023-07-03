#pragma once

#include <remodule/remodule_export.hpp>

namespace remodule {
	enum class module_type {
		SIMPLE = 0,
		CONTEXT,
	};

	class REMODULE_EXPORT module_interface {
	public:
		virtual module_type get_type() const;
		virtual ~module_interface() = default;
	};
}
