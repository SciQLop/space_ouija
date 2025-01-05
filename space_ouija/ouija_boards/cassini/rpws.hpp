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
    const uint64_t seconds = static_cast<uint64_t>(sclk_scet.sclk.SCLK_SECOND) - 378691200ULL;
    const uint64_t microseconds
        = static_cast<uint64_t>(sclk_scet.sclk.SCLK_FINE / 32) * 1000'000ULL / 8ULL
        + static_cast<uint64_t>(SUB_RTI) * 1000ULL;
    return (seconds * 1000'000'000) + (microseconds * 1000);
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
