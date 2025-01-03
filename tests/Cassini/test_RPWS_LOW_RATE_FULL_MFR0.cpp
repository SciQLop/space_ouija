#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cpp_utils.hpp>
#include <string>
#include <filesystem>

#include <ouija_boards/cassini/RPWS_LOW_RATE_FULL_MFR0.hpp>
#include <cpp_utils/io/memory_mapped_file.hpp>

using namespace ouija_boards::cassini::rpws;
TEST_CASE("RPWS_LOW_RATE_FULL_MFR0", "[simple structures]")
{
    cpp_utils::io::memory_mapped_file file(std::filesystem::path(RESOURCES_DIR) / "T2002292_MFR0.DAT");
    auto s = cpp_utils::serde::deserialize<RPWS_LOW_RATE_FULL_MFR0>(file.view(0));
    REQUIRE(s.lrfull_table.RECORD_LENGTH == 912);
    REQUIRE(s.lrfull_table.RECORDS == 8);
}


