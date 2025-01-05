#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cpp_utils.hpp>
#include <string>
#include <filesystem>

#include <ouija_boards/cassini/RPWS_WIDEBAND_FULL_WBRFR.hpp>
#include <cpp_utils/io/memory_mapped_file.hpp>

using namespace ouija_boards::cassini::rpws;
TEST_CASE("RPWS_WIDEBAND_FULL_WBRFR", "[simple structures]")
{
    auto s = load_RPWS_WBR_WFR(std::filesystem::path(RESOURCES_DIR) / "T2009325_02_75KHZ2_WBRFR.DAT");
    REQUIRE(std::size(s.rows)==4);
}


