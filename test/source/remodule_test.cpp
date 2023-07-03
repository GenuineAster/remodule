#define CATCH_CONFIG_FAST_COMPILE

#include <catch2/catch.hpp>

#include <remodule/context_loader.hpp>

#include "common.hpp"

#include <string>

#if defined(_WIN32)
static const char *s_library_suffix = ".dll";
#elif defined(__linux__)
static const char *s_library_suffix = ".so";
#endif

TEST_CASE("Full test", "[full_test]") {
	context ctx;
	remodule::context_loader<context> loader(&ctx);
	auto result = loader.load_module((std::string{"test_module_a"} + s_library_suffix).c_str());
	REQUIRE(result.status == remodule::load_module_status::SUCCESS);
	REQUIRE(loader.get_module_status(result.handle) == remodule::module_status::LOADED);
	auto init_result = loader.init_module(result.handle);
	REQUIRE(init_result.status == remodule::init_module_status::SUCCESS);
	auto unload_result = loader.unload_module(result.handle);
	REQUIRE(unload_result.status == remodule::unload_module_status::SUCCESS);
}
