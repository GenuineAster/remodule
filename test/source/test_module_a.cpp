#include "common.hpp"

#include <remodule/context_module_interface.hpp>

#if defined(_WIN32)
#define MODULE_EXPORT __declspec(dllexport)
#elif defined(__linux__)
#define MODULE_EXPORT __attribute__((__visibility__("default")))
#endif

extern "C" {
	MODULE_EXPORT void* remodule_allocate_module();
	MODULE_EXPORT void remodule_free_module(void *module);
}

class test_module_a final : public remodule::context_module_interface<context> {
public:
	virtual void set_context(context *context) override {
		m_context = context;
	}

private:
	context *m_context = nullptr;
};

extern "C" {
	void* remodule_allocate_module() {
		return reinterpret_cast<void*>(new test_module_a);
	}
	void remodule_free_module(void *module) {
		delete reinterpret_cast<test_module_a*>(module);
	}
}
