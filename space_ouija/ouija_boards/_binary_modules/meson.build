fs.copyfile('__init__.py' ,'__init__.py')

foreach name, board:boards
    python3.extension_module(name, board['sources'],
                             dependencies: [space_ouija_dep, cpp_utils, fmt_dep, pybind11_dep, python3.dependency()],
                             include_directories : board['include_directories'],
                             link_args: link_args,
                             cpp_args: space_ouija_cpp_args+['-DSPACE_OUIJA_PYTHON_BINDINGS'],
                             subdir: 'space_ouija/ouija_boards/_binary_modules',
                             install: true,
                             extra_files: [board['headers']]
    )
endforeach
