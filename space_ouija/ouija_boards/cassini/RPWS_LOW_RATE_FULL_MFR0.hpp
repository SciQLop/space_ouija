/*------------------------------------------------------------------------------
-- The MIT License (MIT)
--
-- Copyright © 2025, Laboratory of Plasma Physics- CNRS
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the “Software”), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
-- of the Software, and to permit persons to whom the Software is furnished to do
-- so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in all
-- copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
-- INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
-- PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
-- HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
-- OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
-- SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
-------------------------------------------------------------------------------*/
/*-- Author : Alexis Jeandet
-- Mail : alexis.jeandet@member.fsf.org
----------------------------------------------------------------------------*/
#include "rpws.hpp"
#include <cpp_utils/reflexion/reflection.hpp>
#include <cpp_utils/serde/serde.hpp>

namespace ouija_boards::cassini::rpws
{

struct TIME_TABLE
{
    using endianness = cpp_utils::endianness::big_endian_t;
    RPWS_SCLK_SCET sclk_scet;
    uint32_t spare;
    cpp_utils::serde::static_array<float, 224> TIME_OFFSET;
};
// static_assert(cpp_utils::reflexion::composite_size<TIME_TABLE>() == 912);

struct FREQUENCY_TABLE
{
    using endianness = cpp_utils::endianness::big_endian_t;
    RPWS_SCLK_SCET sclk_scet;
    uint32_t spare;
    cpp_utils::serde::static_array<float, 224> FREQUENCY;
};
// static_assert(cpp_utils::reflexion::composite_size<FREQUENCY_TABLE>() == 912);

struct LRFC_DATA_QUALITY
{
    using endianness = cpp_utils::endianness::big_endian_t;
    uint32_t value;
    /*inline bool VALID_DATA_FLAG() { return value & 0x1; }
    inline bool HFR_SOUNDER_ACTIVE() { return value & 0x2; }
    inline bool LP_RAW_SWEEP_ACTIVE() { return value & 0x4; }
    inline bool GROUND_PRODUCED_DATA() { return value & 0x8; }
    inline uint8_t SENSOR_NUMBER() { return (value & 0xF000'0000) >> 4; }*/
};

struct SPECTRAL_DENSITY_TABLE
{
    using endianness = cpp_utils::endianness::big_endian_t;
    RPWS_SCLK_SCET sclk_scet;
    LRFC_DATA_QUALITY data_quality;
    cpp_utils::serde::static_array<float, 224> DENSITY;
};
// static_assert(cpp_utils::reflexion::composite_size<SPECTRAL_DENSITY_TABLE>() == 912);

struct RPWS_LOW_RATE_FULL_MFR0
{
    using endianness = cpp_utils::endianness::big_endian_t;
    LRFULL_TABLE<cpp_utils::reflexion::composite_size<TIME_TABLE>()> lrfull_table;
    TIME_TABLE time_table;
    FREQUENCY_TABLE frequency_table;
    cpp_utils::serde::dynamic_array<0, SPECTRAL_DENSITY_TABLE> spectral_density_tables;
    inline std::size_t field_size(
        const cpp_utils::serde::dynamic_array<0, SPECTRAL_DENSITY_TABLE>&) const
    {
        return lrfull_table.RECORDS - 3;
    }
};
}
