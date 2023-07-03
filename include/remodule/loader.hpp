#pragma once

#include <remodule/remodule_export.hpp>

#include <remodule/library_fwd.hpp>
#include <remodule/module_interface_fwd.hpp>

#include <string>
#include <unordered_map>

namespace remodule {
	using module_handle = std::uint64_t;

	using allocate_module_func = void*();
	using free_module_func = void(void*);

	enum class load_module_status {
		SUCCESS = 0,
		LIBRARY_LOAD_FAILED,
		ALLOCATE_FUNC_MISSING,
		FREE_FUNC_MISSING
	};

	struct load_module_result {
		load_module_status status;
		module_handle handle;
	};

	enum class init_module_status {
		SUCCESS = 0,
		MODULE_NOT_FOUND,
		INTERFACE_INIT_FAILED
	};

	struct init_module_result {
		init_module_status status;
	};

	enum class unload_module_status {
		SUCCESS = 0,
		MODULE_NOT_FOUND,
		LIBRARY_UNLOAD_FAILED,
	};

	struct unload_module_result {
		unload_module_status status;
	};

	enum class module_status {
		UNLOADED = 0,
		LOADED = 1,
	};

	class REMODULE_EXPORT loader {
	public:
		static constexpr module_handle c_invalid_module_handle = 0;

		loader();
		virtual ~loader();

		virtual bool has_module(const char *path) const;
		virtual module_handle get_loaded_module(const char *path) const;
		virtual load_module_result load_module(const char *path);
		virtual init_module_result init_module(const module_handle handle);
		virtual module_status get_module_status(const module_handle handle) const;
		virtual unload_module_result unload_module(const module_handle handle);

	protected:
		struct loaded_module {
			module_interface* interface = nullptr;
			module_status status;
			std::string path;
			module_handle handle;
			platform_library *library = nullptr;
			allocate_module_func *allocate_func = nullptr;
			free_module_func *free_func = nullptr;
		};

		std::unordered_map<module_handle, loaded_module> m_modules;

	private:
		module_handle m_handle_counter;
	};
}
