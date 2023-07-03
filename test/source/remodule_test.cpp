#define CATCH_CONFIG_FAST_COMPILE 

#include <catch2/catch.hpp>

#include <string>
#include <remodule/remodule.hpp>
#include <remodule/loader.hpp>

#include "common.hpp"

#if defined(_WIN32)
static const char *s_library_suffix = ".dll";
#elif defined(__linux__)
static const char *s_library_suffix = ".so";
#endif

TEST_CASE("Full test", "[full_test]") {
	context ctx;
	remodule::loader<context> loader(&ctx);
	auto result = loader.load_module(std::string{"test_module_a"} + s_library_suffix);
	REQUIRE(result.status == remodule::load_module_status::SUCCESS);
	REQUIRE(loader.get_module_status(result.handle) == remodule::module_status::LOADED);
	auto init_result = loader.init_module(result.handle);
	REQUIRE(init_result.status == remodule::init_module_status::SUCCESS);
}
