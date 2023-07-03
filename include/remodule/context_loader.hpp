#pragma once

#include <remodule/remodule_export.hpp>

#include <remodule/loader.hpp>
#include <remodule/context_module_interface.hpp>

#include <unordered_map>

namespace remodule {
	template<class T>
	class REMODULE_EXPORT context_loader final : public loader {
	public:
		using context_type = T;
		using module_interface_type = context_module_interface<T>;

		context_loader(context_type *context)
		: m_context{context}
		{}
		~context_loader() = default;

		virtual init_module_result init_module(const module_handle handle) override {
			auto result = loader::init_module(handle);
			if (result.status == init_module_status::SUCCESS) {
				if (auto module_itr = m_modules.find(handle); module_itr != m_modules.end()) {
					if (module_itr->second.interface->get_type() == module_type::CONTEXT) {
						dynamic_cast<module_interface_type*>(module_itr->second.interface)->set_context(m_context);
					}
				}
			}
			return result;
		}

	private:
		context_type *m_context = nullptr;
	};
}
