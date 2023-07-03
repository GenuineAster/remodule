#pragma once

#include <remodule/remodule_export.hpp>
#include <unordered_map>
#include <filesystem>

#include <remodule/library.hpp>

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

	template<class T>
	class REMODULE_EXPORT module_interface {
	public:
		virtual void set_context(T* context) = 0;
		virtual ~module_interface() = default;
	};

	template<class T>
	class REMODULE_EXPORT loader final {
	public:
		static constexpr module_handle c_invalid_module_handle = 0;
		using context_type = T;

		loader(context_type *context)
		: m_context{context}
		{}
		~loader() = default;

		bool has_module(const std::filesystem::path &path) const {
			const auto canonical = std::filesystem::canonical(path);
			for (const auto &[handle, mod] : m_modules) {
				if (mod.path == canonical) {
					return true;
				}
			}

			return false;
		}

		module_handle get_loaded_module(const std::filesystem::path &path) const {
			const auto canonical = std::filesystem::canonical(path);
			for (const auto &[handle, mod] : m_modules) {
				if (mod.path == canonical) {
					return mod.handle;
				}
			}

			return c_invalid_module_handle;
		}

		load_module_result load_module(const std::filesystem::path &path) {
			const auto canonical_path = std::filesystem::canonical(path);
			auto load_result = remodule::load_library(
				reinterpret_cast<const char*>(canonical_path.u8string().c_str())
			);
			if (load_result.status == load_library_status::SUCCESS) {
				loaded_module mod;
				mod.handle = ++m_handle_counter;
				mod.path = canonical_path;
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

		init_module_result init_module(const module_handle handle) {
			if (auto module_itr = m_modules.find(handle); module_itr != m_modules.end()) {
				auto &mod = module_itr->second;
				module_interface<context_type>* interface = reinterpret_cast<module_interface<context_type>*>(mod.allocate_func());
				if (interface != nullptr) {
					mod.interface = interface;
					mod.interface->set_context(m_context);
				} else {
					return {init_module_status::ERROR};
				}
				return {init_module_status::SUCCESS};
			} else {
				return {init_module_status::ERROR};
			}
		}

		module_status get_module_status(const module_handle handle) const {
			if (const auto module_itr = m_modules.find(handle); module_itr != m_modules.end()) {
				return module_itr->second.status;
			}

			return module_status::UNLOADED;
		}

	private:
		struct loaded_module {
			module_interface<context_type>* interface;
			module_status status;
			std::filesystem::path path;
			module_handle handle;
			platform_library *library;
			allocate_module_func *allocate_func;
			free_module_func *free_func;
		};

		context_type *m_context = nullptr;
		module_handle m_handle_counter;
		std::unordered_map<module_handle, loaded_module> m_modules;
	};
}
