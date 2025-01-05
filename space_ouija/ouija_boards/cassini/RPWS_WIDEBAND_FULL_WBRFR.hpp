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

struct RPWS_WBR_WFR_ROW_PREFIX
{
    using endianness = cpp_utils::endianness::big_endian_t;
    RPWS_SCLK_SCET sclk_scet;
    uint16_t RECORD_BYTES;
    uint16_t SAMPLES;
    uint16_t DATA_RTI;
    uint8_t VALIDITY_FLAG;
    uint8_t STATUS_FLAG;
    uint8_t FREQUENCY_BAND;
    uint8_t GAIN;
    uint8_t ANTENNA;
    uint8_t AGC;
    uint8_t HFR_XLATE;
    uint8_t SUB_RTI;
    uint8_t LP_DAC_0;
    uint8_t LP_DAC_1;
    uint8_t FSW_VER;
    cpp_utils::serde::static_array<uint8_t, 3> SPARE;
};
static_assert(cpp_utils::reflexion::composite_size<RPWS_WBR_WFR_ROW_PREFIX>() == 32);

struct RPWS_WBR_WFR_ROW
{
    using endianness = cpp_utils::endianness::big_endian_t;
    RPWS_WBR_WFR_ROW_PREFIX prefix;
    cpp_utils::serde::static_array<uint8_t, 1536> TIME_SERIES;
    cpp_utils::serde::dynamic_array<0, uint8_t> padding;
    std::size_t field_size(const cpp_utils::serde::dynamic_array<0, uint8_t>&) const
    {
        return prefix.RECORD_BYTES - 1536
            - cpp_utils::reflexion::composite_size<RPWS_WBR_WFR_ROW_PREFIX>();
    }
};

struct RPWS_WBR_WFR
{
    using endianness = cpp_utils::endianness::big_endian_t;
    cpp_utils::serde::dynamic_array_until_eof<RPWS_WBR_WFR_ROW> rows;
};
static_assert(cpp_utils::reflexion::is_dyn_size_field_v<decltype(RPWS_WBR_WFR{}.rows)>);
static_assert(!cpp_utils::serde::const_size_field<decltype(RPWS_WBR_WFR{}.rows)>);
static_assert(!cpp_utils::serde::const_size_field<decltype(RPWS_WBR_WFR_ROW{}.padding)>);
static_assert(!cpp_utils::reflexion::composite_have_const_size<RPWS_WBR_WFR_ROW>());
static_assert(std::is_compound_v<RPWS_WBR_WFR_ROW>);
static_assert(!std::is_array_v<RPWS_WBR_WFR_ROW>);
static_assert(!cpp_utils::reflexion::is_field_v<RPWS_WBR_WFR_ROW>);


inline auto load_RPWS_WBR_WFR(const std::string& path)
{
    auto f = cpp_utils::io::memory_mapped_file(path);
    return cpp_utils::serde::deserialize<RPWS_WBR_WFR>(f);
}

#ifdef SPACE_OUIJA_PYTHON_BINDINGS
#include <pybind11/pybind11.h>

#include "py_array.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


inline auto py_load_RPWS_WBR_WFR(const std::string& path)
{
    namespace py = pybind11;
    auto s = load_RPWS_WBR_WFR(path);
    py::dict d;
    {
        auto time_series = py_create_ndarray<uint8_t>(std::size(s.rows), 1536);
        auto antennas = py_create_ndarray<uint8_t>(std::size(s.rows));
        auto gain = py_create_ndarray<uint8_t>(std::size(s.rows));

        for_each_block(s.rows,
            [&, i = 0](const auto& row) mutable
            {
                copy_values(row.TIME_SERIES, time_series, i * 1536);
                antennas.mutable_data()[i] = row.prefix.ANTENNA;
                gain.mutable_data()[i] = row.prefix.GAIN;
                i++;
            });
        d["time_series"] = std::move(time_series);
        d["antennas"] = std::move(antennas);
        d["gain"] = std::move(gain);
    }
    return d;
}

inline void py_register_RPWS_WBR_WFR(py::module& m)
{
    m.def("load_RPWS_WBR_WFR", &py_load_RPWS_WBR_WFR);
    py::enum_<ANTENNA_t>(m, "ANTENNA")
        .value("Ex", ANTENNA_t::Ex)
        .value("Eu", ANTENNA_t::Eu)
        .value("Ev", ANTENNA_t::Ev)
        .value("Ew", ANTENNA_t::Ew)
        .value("Bx", ANTENNA_t::Bx)
        .value("By", ANTENNA_t::By)
        .value("Bz", ANTENNA_t::Bz)
        .value("Hf", ANTENNA_t::Hf)
        .value("Lp", ANTENNA_t::Lp)
        .value("Unknown", ANTENNA_t::Unknown);
}
#endif

}
