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

#include "rpws.hpp"
#include <cpp_utils/io/memory_mapped_file.hpp>
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
static_assert(cpp_utils::reflexion::composite_size<TIME_TABLE>() == 912);

struct FREQUENCY_TABLE
{
    using endianness = cpp_utils::endianness::big_endian_t;
    RPWS_SCLK_SCET sclk_scet;
    uint32_t spare;
    cpp_utils::serde::static_array<float, 224> FREQUENCY;
};
static_assert(cpp_utils::reflexion::composite_size<FREQUENCY_TABLE>() == 912);

struct LRFC_DATA_QUALITY
{
    using endianness = cpp_utils::endianness::big_endian_t;
    uint32_t value;
    inline bool VALID_DATA_FLAG() { return value & 0x1; }
    inline bool HFR_SOUNDER_ACTIVE() { return value & 0x2; }
    inline bool LP_RAW_SWEEP_ACTIVE() { return value & 0x4; }
    inline bool GROUND_PRODUCED_DATA() { return value & 0x8; }
    inline uint8_t SENSOR_NUMBER() { return (value & 0xF000'0000) >> 4; }
};

struct SPECTRAL_DENSITY_TABLE
{
    using endianness = cpp_utils::endianness::big_endian_t;
    RPWS_SCLK_SCET sclk_scet;
    LRFC_DATA_QUALITY data_quality;
    cpp_utils::serde::static_array<float, 224> DENSITY;
};
static_assert(cpp_utils::reflexion::composite_size<SPECTRAL_DENSITY_TABLE>() == 912);

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

inline auto load_RPWS_LOW_RATE_FULL_MFR0(const std::string& path)
{
    return cpp_utils::serde::deserialize<RPWS_LOW_RATE_FULL_MFR0>(
        cpp_utils::io::memory_mapped_file(path).view(0));
}

#ifdef SPACE_OUIJA_PYTHON_BINDINGS
#include <pybind11/pybind11.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

template <typename T>
concept py_array_interface = requires(T t) {
    t.data();
    t.shape();
    t.strides();
    t.mutable_data();
};

template <typename T>
inline auto py_create_ndarray(auto... shape)
{
    namespace py = pybind11;
    return py::array_t<T>({static_cast<py::ssize_t>(shape)...});
}

inline void copy_values(const auto& src, py_array_interface auto& dst, uint64_t offset = 0)
{
    std::memcpy(dst.mutable_data() + offset, src.data(), src.size() * sizeof(decltype(src[0])));
}

inline void for_each_block(const auto& src, auto&& f)
{
    std::for_each(std::begin(src), std::end(src), f);
}

inline void transform_values(const auto& src, py_array_interface auto& dst, auto&& f)
{
    std::transform(
        std::begin(src), std::end(src), dst.mutable_data(), std::forward<decltype(f)>(f));
}

uint64_t cassini_time_to_ns_since_epoch(const SPECTRAL_DENSITY_TABLE& density)
{
    return cassini_time_to_ns_since_epoch(density.sclk_scet.sclk, density.sclk_scet.scet);
}

inline auto py_load_RPWS_LOW_RATE_FULL_MFR0(const std::string& path)
{
    namespace py = pybind11;
    auto s = load_RPWS_LOW_RATE_FULL_MFR0(path);
    py::dict d;
    const auto values_count = s.frequency_table.FREQUENCY.size();
    const auto density_tables_count = std::size(s.spectral_density_tables);
    auto spectral_density = py_create_ndarray<float>(density_tables_count, values_count);
    auto time = py_create_ndarray<uint64_t>(density_tables_count);
    transform_values(s.spectral_density_tables, time,
        [](const auto& density) { return cassini_time_to_ns_since_epoch(density); });
    auto frequency = py_create_ndarray<float>(values_count);
    copy_values(s.frequency_table.FREQUENCY, frequency);
    for_each_block(s.spectral_density_tables,
        [&, global_offset = 0ULL](const auto& table) mutable
        {
            copy_values(table.DENSITY, spectral_density, global_offset);
            global_offset += values_count;
        });
    d["time"] = std::move(time);
    d["frequency"] = std::move(frequency);
    d["spectral_density"] = std::move(spectral_density);
    return d;
}

#endif

} // namespace ouija_boards::cassini::rpws
