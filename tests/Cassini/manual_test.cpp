#include <array>
#include <cpp_utils.hpp>
#include <filesystem>
#include <iostream>
#include <string>

#include <cpp_utils/io/memory_mapped_file.hpp>
#include <ouija_boards/cassini/RPWS_LOW_RATE_FULL_MFR0.hpp>
#include <ouija_boards/cassini/RPWS_WAVEFORM_FULL.hpp>
#include <ouija_boards/cassini/RPWS_WIDEBAND_FULL_WBRFR.hpp>

using namespace ouija_boards::cassini::rpws;
int main()
{
    auto files = std::filesystem::directory_iterator("/home/jeandet/Downloads/");
    for (const auto& file : files)
    {
        auto path = file.path().string();
        if (path.find("25HZ1_WFRFR.DAT") != std::string::npos)
        {
            auto s
                = cpp_utils::serde::deserialize<RPWS_WFR>(cpp_utils::io::memory_mapped_file(path));
            auto samples = 0;
            if (std::size(s.rows) != 0)
                samples = s.rows[0].prefix.SAMPLES;
            std::cout << path << "\tWFRFR: " << std::size(s.rows) << "\trows"
                      << "\twith " << samples << "\tsamples" << std::endl;
        }
    }
    return 0;
}
