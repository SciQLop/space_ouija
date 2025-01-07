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
#pragma once
#include <cpp_utils/endianness/endianness.hpp>
#include <cpp_utils/reflexion/reflection.hpp>
#include <cpp_utils/serde/special_fields.hpp>
#include <cstdint>

namespace ouija_boards::cassini::rpws
{

template <std::size_t record_size>
struct LRFULL_TABLE
{
    using endianness = cpp_utils::endianness::big_endian_t;
    cpp_utils::serde::static_array<char, 8> FILE_ID;
    uint32_t RECORD_LENGTH;
    uint32_t RECORDS;
    uint32_t RECEIVER_TYPE;
    uint32_t unused;
    cpp_utils::serde::static_array<char, 24> MINI_PACKET_HEADER;
    cpp_utils::serde::static_array<char, 16> SCET;
    cpp_utils::serde::static_array<char, 16> SCLK;
    cpp_utils::serde::static_array<char, record_size - 80> extra_space;
};

struct RPWS_SCLK
{
    using endianness = cpp_utils::endianness::big_endian_t;
    uint32_t SCLK_SECOND;
    uint8_t SCLK_PARTITION;
    uint8_t SCLK_FINE;
};

static_assert(cpp_utils::reflexion::composite_size<RPWS_SCLK>() == 6);

struct RPWS_SCET
{
    using endianness = cpp_utils::endianness::big_endian_t;
    uint16_t SCET_DAY;
    uint32_t SCET_MILLISECOND;
};
static_assert(cpp_utils::reflexion::composite_size<RPWS_SCET>() == 6);


struct RPWS_SCLK_SCET
{
    using endianness = cpp_utils::endianness::big_endian_t;
    RPWS_SCLK sclk;
    RPWS_SCET scet;
};
static_assert(cpp_utils::reflexion::composite_size<RPWS_SCLK_SCET>() == 12);


enum class ANTENNA_t : uint8_t
{
    /*0 = Ex, electric dipole X-direction
     1 = Eu, electric U-direction (aka Ex+) (WFR only)
     2 = Ev, electric V-direction (aka Ex-) (WFR only)
     3 = Ew, electric W-direction (aka Ez)
     4 = Bx, magnetic X-direction
     5 = By, magnetic Y-direction           (WFR only)
     6 = Bz, magnetic Z-direction           (WFR only)
     8 = HF, HFR downconvert                (WBR only)
    11 = LP, Langmuir probe sphere
    15 = unknown, antenna cannot be determined"
    */
    Ex = 0,
    Eu = 1,
    Ev = 2,
    Ew = 3,
    Bx = 4,
    By = 5,
    Bz = 6,
    Hf = 8,
    Lp = 11,
    Unknown = 15
};

enum class FREQUENCY_BAND_t : uint8_t
{
    /*
    0 = 26 Hz, 10 millisecond sample period          (WFR only)
    1 = 2.5 KHz, 140 microsecond sample period       (WFR only)
    2 = 10 KHz filter, 36 microsecond sample period  (WBR only)
    3 = 80 KHz filter, 4.5 microsecond sample period (WBR only)"
    */
    Hz26 = 0,
    KHz2_5 = 1,
    KHz10 = 2,
    KHz80 = 3
};

inline double frequency_band_to_dt(FREQUENCY_BAND_t band)
{
    switch (band)
    {
        case FREQUENCY_BAND_t::Hz26:
            return 10e-3;
        case FREQUENCY_BAND_t::KHz2_5:
            return 140e-6;
        case FREQUENCY_BAND_t::KHz10:
            return 36e-6;
        case FREQUENCY_BAND_t::KHz80:
            return 4.5e-6;
    }
    return 0;
}

inline uint8_t decode_gain(uint8_t gain)
{
    auto WALSH_DGF = ((gain >> 3) & 0x3) * 6;
    auto ANALOG_GAIN = ((gain >> 6) & 0x7) * 10;
    return WALSH_DGF + ANALOG_GAIN;
}

struct RPWS_WBR_WFR_ROW_PREFIX
{
    using endianness = cpp_utils::endianness::big_endian_t;
    RPWS_SCLK_SCET sclk_scet;
    uint16_t RECORD_BYTES;
    uint16_t SAMPLES;
    uint16_t DATA_RTI;
    uint8_t VALIDITY_FLAG;
    uint8_t STATUS_FLAG;
    FREQUENCY_BAND_t FREQUENCY_BAND;
    uint8_t GAIN;
    ANTENNA_t ANTENNA;
    uint8_t AGC;
    uint8_t HFR_XLATE;
    uint8_t SUB_RTI;
    uint8_t LP_DAC_0;
    uint8_t LP_DAC_1;
    uint8_t FSW_VER;
    cpp_utils::serde::static_array<uint8_t, 3> SPARE;
};
static_assert(cpp_utils::reflexion::composite_size<RPWS_WBR_WFR_ROW_PREFIX>() == 32);




template <typename T>
concept block_with_sclk_scet_and_sub_rti = requires(T t) {
    { t.sclk_scet } -> std::convertible_to<RPWS_SCLK_SCET>;
    { t.SUB_RTI } -> std::convertible_to<uint32_t>;
};

template <typename T>
concept block_with_sclk_scet = requires(T t) {
    { t.sclk_scet } -> std::convertible_to<RPWS_SCLK_SCET>;
} && !block_with_sclk_scet_and_sub_rti<T>;


inline uint64_t cassini_time_to_ns_since_epoch(
    const RPWS_SCLK_SCET& sclk_scet, uint32_t SUB_RTI = 0)
{
    const uint64_t seconds = static_cast<uint64_t>(sclk_scet.sclk.SCLK_SECOND) - 378773032ULL;
    const uint64_t microseconds = 0;
    /*    = static_cast<uint64_t>(sclk_scet.sclk.SCLK_FINE / 32) * 1000'000ULL / 8ULL
        + static_cast<uint64_t>(SUB_RTI) * 1000ULL;*/
    return ((seconds * 1000'000) + (microseconds)) * 999'951'422/ 1'000'000;
}

inline uint64_t cassini_time_to_ns_since_epoch(const block_with_sclk_scet_and_sub_rti auto& block)
{
    return cassini_time_to_ns_since_epoch(block.sclk_scet, block.SUB_RTI);
}

inline uint64_t cassini_time_to_ns_since_epoch(const block_with_sclk_scet auto& block)
{
    return cassini_time_to_ns_since_epoch(block.sclk_scet);
}
} // namespace ouija_boards::cassini::rpws
