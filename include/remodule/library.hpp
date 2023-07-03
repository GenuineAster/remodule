#pragma once

#include <remodule/remodule_export.hpp>

#include <remodule/library_fwd.hpp>

namespace remodule {
	struct platform_library;

	enum class load_library_status {
		SUCCESS = 0,
		ERROR,
	};

	struct load_library_result {
		load_library_status status;
		const char *error;
		platform_library *library;
	};

	enum class unload_library_status {
		SUCCESS = 0,
		ERROR
	};

	struct unload_library_result {
		unload_library_status status;
		const char *error;
	};

	enum class get_func_addr_status {
		SUCCESS = 0,
		ERROR
	};

	struct get_func_addr_result {
		get_func_addr_status status;
		const char *error;
		void *func_addr;
	};

	REMODULE_EXPORT load_library_result load_library(const char *path);
	REMODULE_EXPORT get_func_addr_result get_func_addr(platform_library *library, const char *func_name);
	REMODULE_EXPORT unload_library_result unload_library(platform_library *library);
}
