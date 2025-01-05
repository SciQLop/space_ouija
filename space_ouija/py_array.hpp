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
#ifdef SPACE_OUIJA_PYTHON_BINDINGS
#include <pybind11/pybind11.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

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
    return py::array_t<T>({ static_cast<py::ssize_t>(shape)... });
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


[[nodiscard]] inline py::object array_to_datetime64(const py_array_interface auto&& input)
{
     return py::cast(&input).attr("astype")("datetime64[ns]");
}

#endif
