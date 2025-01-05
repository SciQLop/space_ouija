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
enum class SENSOR_NUMBER
{
    Ex = 0,
    Eu = 1,
    Ev = 2,
    Ew = 3,
    Bx = 4,
    By = 5,
    Bz = 6,
    Hf = 8,
    Lp = 11,
};

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
    inline bool VALID_DATA_FLAG() const { return value & (0x1 << 24); }
    inline bool HFR_SOUNDER_ACTIVE() const { return value & (0x2 << 24); }
    inline bool LP_RAW_SWEEP_ACTIVE() const { return value & (0x4 << 24); }
    inline bool GROUND_PRODUCED_DATA() const { return value & (0x8 << 24); }
    inline SENSOR_NUMBER sensor_number() const { return static_cast<SENSOR_NUMBER>(value & 255); }
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
#include "py_array.hpp"

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
    {
        auto time = py_create_ndarray<uint64_t>(density_tables_count);
        transform_values(s.spectral_density_tables, time,
            [](const auto& density) { return cassini_time_to_ns_since_epoch(density); });
        d["time"] = std::move(time);
    }
    {
        auto frequency = py_create_ndarray<float>(values_count);
        copy_values(s.frequency_table.FREQUENCY, frequency);
        d["frequency"] = std::move(frequency);
    }
    {
        auto spectral_density = py_create_ndarray<float>(density_tables_count, values_count);
        for_each_block(s.spectral_density_tables,
            [&, global_offset = 0ULL](const auto& table) mutable
            {
                copy_values(table.DENSITY, spectral_density, global_offset);
                global_offset += values_count;
            });
        d["spectral_density"] = std::move(spectral_density);
    }
    {
        auto sensor_number = py_create_ndarray<SENSOR_NUMBER>(density_tables_count);
        transform_values(s.spectral_density_tables, sensor_number,
            [](const auto& density) { return density.data_quality.sensor_number(); });
        d["sensor_number"] = std::move(sensor_number);
    }
    return d;
}

inline void py_register_RPWS_LOW_RATE_FULL_MFR0(py::module& m)
{
    m.def("load_RPWS_LOW_RATE_FULL_MFR0", &py_load_RPWS_LOW_RATE_FULL_MFR0);
    py::enum_<SENSOR_NUMBER>(m, "SENSOR_NUMBER")
        .value("Ex", SENSOR_NUMBER::Ex)
        .value("Eu", SENSOR_NUMBER::Eu)
        .value("Ev", SENSOR_NUMBER::Ev)
        .value("Ew", SENSOR_NUMBER::Ew)
        .value("Bx", SENSOR_NUMBER::Bx)
        .value("By", SENSOR_NUMBER::By)
        .value("Bz", SENSOR_NUMBER::Bz)
        .value("Hf", SENSOR_NUMBER::Hf)
        .value("Lp", SENSOR_NUMBER::Lp);

}

#endif

} // namespace ouija_boards::cassini::rpws
