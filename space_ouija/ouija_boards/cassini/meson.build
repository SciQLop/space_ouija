fs.copyfile('__init__.py' ,'__init__.py')

cassini_headers = files(
    'rpws.hpp',
    'RPWS_LOW_RATE_FULL_MFR0.hpp',
    'RPWS_WIDEBAND_FULL_WBRFR.hpp',
    'RPWS_WAVEFORM_FULL.hpp',
    'cassini.hpp'
)

_cassini = python3.extension_module('_cassini', 'cassini.cpp',
                         dependencies: [space_ouija_dep, cpp_utils, fmt_dep, pybind11_dep, python3.dependency()],
                         include_directories : include_directories('.'),
                         link_args: link_args,
                         cpp_args: space_ouija_cpp_args+['-DSPACE_OUIJA_PYTHON_BINDINGS'],
                         subdir: 'space_ouija',
                         install: true,
                         extra_files: [cassini_headers]
)
