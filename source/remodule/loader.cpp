#include <remodule/loader.hpp>

#include <remodule/library.hpp>

#include <filesystem>

namespace remodule {
	loader::loader() = default;
	loader::~loader() = default;

	bool loader::has_module(const char *path) const {
		const auto canonical = std::filesystem::canonical(path);
		for (const auto &[handle, mod] : m_modules) {
			if (mod.path == canonical) {
				return true;
			}
		}

		return false;
	}

	module_handle loader::get_loaded_module(const char *path) const {
		const auto canonical = std::filesystem::canonical(path);
		for (const auto &[handle, mod] : m_modules) {
			if (mod.path == canonical) {
				return mod.handle;
			}
		}

		return c_invalid_module_handle;
	}

	load_module_result loader::load_module(const char *path) {
		const auto canonical_path = std::filesystem::canonical(path);
		auto load_result = remodule::load_library(path);
		if (load_result.status == load_library_status::SUCCESS) {
			loaded_module mod;
			mod.handle = ++m_handle_counter;
			mod.path = canonical_path.string();
			mod.status = module_status::LOADED;
			mod.library = load_result.library;

			auto allocate_func_result = remodule::get_func_addr(mod.library, "remodule_allocate_module");
			if (allocate_func_result.status == get_func_addr_status::SUCCESS) {
				mod.allocate_func = reinterpret_cast<allocate_module_func*>(allocate_func_result.func_addr);
			} else {
				remodule::unload_library(mod.library);
				return {load_module_status::ERROR, c_invalid_module_handle};
			}

			auto free_func_result = remodule::get_func_addr(mod.library, "remodule_free_module");
			if (free_func_result.status == get_func_addr_status::SUCCESS) {
				mod.free_func = reinterpret_cast<free_module_func*>(free_func_result.func_addr);
			} else {
				remodule::unload_library(mod.library);
				return {load_module_status::ERROR, c_invalid_module_handle};
			}

			m_modules.emplace(std::make_pair(mod.handle, std::move(mod)));

			return {load_module_status::SUCCESS, m_handle_counter};
		} else {
			return {load_module_status::ERROR, c_invalid_module_handle};
		}
	}

	init_module_result loader::init_module(const module_handle handle) {
		if (auto module_itr = m_modules.find(handle); module_itr != m_modules.end()) {
			auto &mod = module_itr->second;
			module_interface* interface = reinterpret_cast<module_interface*>(mod.allocate_func());
			if (interface != nullptr) {
				mod.interface = interface;
			} else {
				return {init_module_status::ERROR};
			}
			return {init_module_status::SUCCESS};
		} else {
			return {init_module_status::ERROR};
		}
	}

	module_status loader::get_module_status(const module_handle handle) const {
		if (const auto module_itr = m_modules.find(handle); module_itr != m_modules.end()) {
			return module_itr->second.status;
		}

		return module_status::UNLOADED;
	}
}
