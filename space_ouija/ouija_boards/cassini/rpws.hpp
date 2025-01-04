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

inline uint64_t cassini_time_to_ns_since_epoch(const RPWS_SCLK& sclk, const RPWS_SCET&)
{
    uint64_t seconds = static_cast<uint64_t>(sclk.SCLK_SECOND) - 378694800ULL;
    return (seconds * 1000'000'000) + (sclk.SCLK_FINE * 1000'000'000 / 256);
}
} // namespace ouija_boards::cassini::rpws
