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

struct RPWS_WFR_ROW
{
    using endianness = cpp_utils::endianness::big_endian_t;
    RPWS_WBR_WFR_ROW_PREFIX prefix;
    cpp_utils::serde::dynamic_array<0, uint16_t> TIME_SERIES;
    std::size_t field_size(const cpp_utils::serde::dynamic_array<0, uint16_t>&) const
    {
        return prefix.SAMPLES;
    }

    cpp_utils::serde::dynamic_array<1, uint8_t> padding;

    std::size_t field_size(const cpp_utils::serde::dynamic_array<1, uint8_t>&) const
    {
        return prefix.RECORD_BYTES - (field_size(TIME_SERIES) * 2)
            - cpp_utils::reflexion::composite_size<RPWS_WBR_WFR_ROW_PREFIX>();
    }
};
static_assert(!cpp_utils::reflexion::composite_have_const_size<RPWS_WFR_ROW>());

struct RPWS_WFR
{
    using endianness = cpp_utils::endianness::big_endian_t;
    cpp_utils::serde::dynamic_array_until_eof<RPWS_WFR_ROW> rows;
};


inline auto load_RPWS_WFR(const std::string& path)
{
    auto f = cpp_utils::io::memory_mapped_file(path);
    return cpp_utils::serde::deserialize<RPWS_WFR>(f);
}

#ifdef SPACE_OUIJA_PYTHON_BINDINGS
#include <pybind11/pybind11.h>

#include "py_array.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


inline auto py_load_RPWS_WFR(const std::string& path)
{
    namespace py = pybind11;
    auto s = load_RPWS_WFR(path);
    py::dict d;
    {
        if (std::size(s.rows) != 0)
        {
            auto max_samples = std::max_element(std::begin(s.rows), std::end(s.rows),
                [](const auto& a, const auto& b)
                {
                    return a.prefix.SAMPLES < b.prefix.SAMPLES;
                })->prefix.SAMPLES;
            const auto sampling_period = frequency_band_to_dt(s.rows[0].prefix.FREQUENCY_BAND);
            auto time_series = py_create_ndarray<uint16_t>(std::size(s.rows), max_samples);
            auto antennas = py_create_ndarray<ANTENNA_t>(std::size(s.rows));
            auto gain = py_create_ndarray<uint8_t>(std::size(s.rows));
            auto time = py_create_ndarray<uint64_t>(std::size(s.rows));
            auto raw_time = py_create_ndarray<uint32_t>(std::size(s.rows));
            auto samples_count = py_create_ndarray<uint16_t>(std::size(s.rows));
            for_each_block(s.rows,
                [&, i = 0](const auto& row) mutable
                {
                    copy_values(row.TIME_SERIES, time_series, i * row.prefix.SAMPLES);
                    samples_count.mutable_data()[i] = row.prefix.SAMPLES;
                    antennas.mutable_data()[i] = row.prefix.ANTENNA;
                    gain.mutable_data()[i] = decode_gain(row.prefix.GAIN);
                    time.mutable_data()[i] = cassini_time_to_ns_since_epoch(row.prefix);
                    raw_time.mutable_data()[i] = row.prefix.sclk_scet.sclk.SCLK_SECOND;
                    i++;
                });
            d["time"] = array_to_datetime64(std::move(time));
            d["raw_time"] = std::move(raw_time);
            d["time_series"] = std::move(time_series);
            d["samples_count"] = std::move(samples_count);
            d["antennas"] = std::move(antennas);
            d["gain"] = std::move(gain);
            d["sampling_period"] = sampling_period;
        }
    }
    return d;
}

inline void py_register_RPWS_WFR(py::module& m)
{
    m.def("load_RPWS_WFR", &py_load_RPWS_WFR);
}
#endif

}
