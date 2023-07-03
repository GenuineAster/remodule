#pragma once

#include <remodule/remodule_export.hpp>

#include <string>
#include <unordered_map>

#include <remodule/library_fwd.hpp>
#include <remodule/module_interface_fwd.hpp>

namespace remodule {
	using module_handle = std::uint64_t;

	using allocate_module_func = void*();
	using free_module_func = void(void*);

	enum class load_module_status {
		SUCCESS = 0,
		ERROR
	};

	struct load_module_result {
		load_module_status status;
		module_handle handle;
	};

	enum class init_module_status {
		SUCCESS = 0,
		ERROR
	};

	struct init_module_result {
		init_module_status status;
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

	protected:
		struct loaded_module {
			module_interface* interface;
			module_status status;
			std::string path;
			module_handle handle;
			platform_library *library;
			allocate_module_func *allocate_func;
			free_module_func *free_func;
		};

		std::unordered_map<module_handle, loaded_module> m_modules;

	private:
		module_handle m_handle_counter;
	};
}
