fs.copyfile('__init__.py' ,'__init__.py')

messenger_headers = files(
    'messenger.hpp',
    'FIPS_EDR.hpp'
)

_messenger = python3.extension_module('_messenger', 'messenger.cpp',
                         dependencies: [space_ouija_dep, cpp_utils, fmt_dep, pybind11_dep, python3.dependency()],
                         include_directories : include_directories('.'),
                         link_args: link_args,
                         cpp_args: space_ouija_cpp_args+['-DSPACE_OUIJA_PYTHON_BINDINGS'],
                         subdir: 'space_ouija/ouija_boards/messenger',
                         install: true,
                         extra_files: [messenger_headers]
)
