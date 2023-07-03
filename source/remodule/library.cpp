#include <remodule/library.hpp>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MIN
	#define NOMINMAX
	#include <windows.h>
	#include <libloaderapi.h>
	#include <errhandlingapi.h>
	#undef ERROR
#elif defined(__linux__)
	#include <dlfcn.h>
#endif

namespace remodule {
	struct platform_library {
#if defined(_WIN32)
		HMODULE library_handle;
#elif defined(__linux__)
		void *library_handle;
#endif
	};

	load_library_result load_library(const char *path) {
#if defined(_WIN32)
		auto handle = LoadLibraryExA(path, nullptr, 0);
		if (handle) {
			// for future memory optimizations, one can call:
			// DisableThreadLibraryCalls(handle);
			return {load_library_status::SUCCESS, nullptr, new platform_library{handle}};
		} else {
			// TODO: win32 error handling
			//const auto err = GetLastError();
			return {load_library_status::ERROR, nullptr, nullptr};
		}
#elif defined(__linux__)
		auto handle = dlopen(path, RTLD_GLOBAL | RTLD_LAZY);
		if (handle) {
			return {load_library_status::SUCCESS, new platform_library{handle}};
		} else {
			const auto err = dlerror();
			return {load_library_status::ERROR, err, nullptr};
		}
#endif
	}

	get_func_addr_result get_func_addr(platform_library *library, const char *func_name) {
#if defined(_WIN32)
		auto func = GetProcAddress(library->library_handle, func_name);
		if (func) {
			return {get_func_addr_status::SUCCESS, nullptr, func};
		} else {
			// TODO: win32 error handling
			//const auto err = GetLastError();
			return {get_func_addr_status::ERROR, nullptr, nullptr};
		}
#elif defined(__linux__)
		auto func = dlsym(plugin_handle, func_name);
		if (auto err = dlerror()) {
			return {get_func_addr_status::ERROR, err, nullptr};
		} else {
			return {get_func_addr_status::SUCCESS, nullptr, func};
		}
#endif
	}

	unload_library_result unload_library(platform_library *library) {
#if defined(_WIN32)
		auto result = FreeLibrary(library->library_handle);
		if (result) {
			return {unload_library_status::SUCCESS, nullptr};
		} else {
			// TODO: win32 error handling
			//const auto err = GetLastError();
			return {unload_library_status::ERROR, nullptr};
		}
#elif defined(__linux__)
		const auto result = dlclose(library.library_handle);
		if (result) {
			auto err = dlerror();
			return {unload_library_status::ERROR, err, nullptr};
		} else {
			return {unload_library_status::SUCCESS, new platform_library{handle}};
		}
#endif
	}
}
