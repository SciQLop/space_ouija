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
#include <algorithm>
#include <filesystem>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#ifndef SPACE_OUIJA_PYTHON_BINDINGS
#define SPACE_OUIJA_PYTHON_BINDINGS
#endif
namespace py = pybind11;
#include "RPWS_LOW_RATE_FULL_MFR0.hpp"
#include "RPWS_WIDEBAND_FULL_WBRFR.hpp"
#include "RPWS_WAVEFORM_FULL.hpp"
#include <space_ouija_config.h>

namespace py = pybind11;
using namespace ouija_boards::cassini::rpws;

PYBIND11_MODULE(_cassini, m)
{
    m.doc() = R"pbdoc(
        _space_ouija
        --------

    )pbdoc";

    m.attr("__version__") = SPACE_OUIJA_VERSION;
    py_register_RPWS_LOW_RATE_FULL_MFR0(m);
    py_register_RPWS_WBR_WFR(m);
    py_register_RPWS_WFR(m);
}
