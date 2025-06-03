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
#include <span>

#ifdef SPACE_OUIJA_PYTHON_BINDINGS
#include <pybind11/pybind11.h>

#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;


#define REGISTER_LOADER(name, module, loader)                                                      \
    {                                                                                              \
        module.def(                                                                                \
            name,                                                                                  \
            [](py::bytes& buffer)                                                                  \
            {                                                                                      \
                py::buffer_info info(py::buffer(buffer).request());                                \
                return loader(                                                                     \
                    std::span<const char>(static_cast<const char*>(info.ptr), info.size));         \
            },                                                                                     \
            py::arg("buffer"), py::return_value_policy::move);                                     \
                                                                                                   \
        module.def(                                                                                \
            name,                                                                                  \
            [](const std::string& path)                                                            \
            {                                                                                      \
                auto f = cpp_utils::io::memory_mapped_file(path);                                  \
                return loader(path);                                                               \
            },                                                                                     \
            py::arg("fname"), py::return_value_policy::move);                                      \
    }\

#endif // SPACE_OUIJA_PYTHON_BINDINGS
