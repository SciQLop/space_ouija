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

#include <cpp_utils/io/memory_mapped_file.hpp>
#include <cpp_utils/reflexion/reflection.hpp>
#include <cpp_utils/serde/serde.hpp>

#include "load_from.hpp"

namespace ouija_boards::messenger::fips
{

enum class FIPS_SCANTYPE
{
    Normal = 0,
    HighTemp = 1,
    Burst = 2,
    Test = 3,
    Table4 = 4,
    Table5 = 5,
    Table6 = 6,
    Table7 = 7
};

struct FIPS_EDR_BLOCK
{
    using endianness = cpp_utils::endianness::big_endian_t;
    uint32_t met;
    uint16_t scantype;
    cpp_utils::serde::static_array<uint32_t, 64> start_rate;
    cpp_utils::serde::static_array<uint32_t, 64> stop_rate;
    cpp_utils::serde::static_array<uint32_t, 64> valid_evt_rate;
    cpp_utils::serde::static_array<uint32_t, 64> proton_rate;
    cpp_utils::serde::static_array<uint32_t, 64> evt_proc_rate;
};
static_assert(cpp_utils::reflexion::composite_size<FIPS_EDR_BLOCK>() == 1286);

struct FIPS_EDR
{
    using endianness = cpp_utils::endianness::big_endian_t;
    cpp_utils::serde::dynamic_array_until_eof<FIPS_EDR_BLOCK> rows;
};

inline auto load_FIPS_EDR(const auto& input)
{
    return load_from<FIPS_EDR>(input);
}


#ifdef SPACE_OUIJA_PYTHON_BINDINGS
#include <pybind11/pybind11.h>

#include "py_array.hpp"
#include "py_load_from.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

inline auto py_load_FIPS_EDR(const auto input)
{
    namespace py = pybind11;
    auto fips_edr_data = [&]()
    {
        py::gil_scoped_release release;
        return load_FIPS_EDR(input);
    }();
    py::dict d;
    const auto row_count = std::size(fips_edr_data.rows);
    {
        auto met = py_create_ndarray<uint32_t>(row_count);
        auto scantype = py_create_ndarray<uint32_t>(row_count);
        auto start_rate = py_create_ndarray<uint32_t>(row_count, 64);
        auto stop_rate = py_create_ndarray<uint32_t>(row_count, 64);
        auto valid_evt_rate = py_create_ndarray<uint32_t>(row_count, 64);
        auto proton_rate = py_create_ndarray<uint32_t>(row_count, 64);
        auto evt_proc_rate = py_create_ndarray<uint32_t>(row_count, 64);
        for_each_block(fips_edr_data.rows,
            [&, global_offset = 0ULL, row_index = 0UL](const FIPS_EDR_BLOCK& row) mutable
            {
                copy_values(row.start_rate, start_rate, global_offset);
                copy_values(row.stop_rate, stop_rate, global_offset);
                copy_values(row.valid_evt_rate, valid_evt_rate, global_offset);
                copy_values(row.proton_rate, proton_rate, global_offset);
                copy_values(row.evt_proc_rate, evt_proc_rate, global_offset);
                met.mutable_data()[row_index] = row.met;
                scantype.mutable_data()[row_index] = row.scantype;
                global_offset += 64;
                row_index++;
            });
        d["met"] = std::move(met);
        d["scantype"] = std::move(scantype);
        d["start_rate"] = std::move(start_rate);
        d["stop_rate"] = std::move(stop_rate);
        d["valid_evt_rate"] = std::move(valid_evt_rate);
        d["proton_rate"] = std::move(proton_rate);
        d["evt_proc_rate"] = std::move(evt_proc_rate);
    }
    return d;
}

inline void py_register_FIPS_EDR(py::module& m)
{
    REGISTER_LOADER("load_FIPS_EDR", m, py_load_FIPS_EDR);
}

#endif

} // namespace ouija_boards::cassini::rpws
