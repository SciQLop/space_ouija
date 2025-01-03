space_ouija_python_sources = [files('__init__.py')]
fs = import('fs')
fs.copyfile('__init__.py', '__init__.py')

pymod = import('python')
python3 = pymod.find_installation('python3')

if meson.get_compiler('cpp').get_id() == 'msvc'
    space_ouija_cpp_args = ['/bigobj']
else
    space_ouija_cpp_args = []
endif

conf_data = configuration_data()
conf_data.set_quoted('SPACE_OUIJA_VERSION', meson.project_version())

configure_file(output : 'space_ouija_config.h',
               configuration : conf_data)

space_ouija_headers = files(
    'ouija_boards/cassini/rpws.hpp',
    'ouija_boards/cassini/RPWS_LOW_RATE_FULL_MFR0.hpp',
)

space_ouija_dep = declare_dependency(
    include_directories : include_directories('.'),
    dependencies : [cpp_utils, fmt_dep]
)

_space_ouija = python3.extension_module('_space_ouija', 'space_ouija.cpp',
                         dependencies: [cpp_utils, fmt_dep, pybind11_dep, python3.dependency()],
                         include_directories : include_directories('.'),
                         link_args: link_args,
                         cpp_args: space_ouija_cpp_args,
                         subdir: 'space_ouija',
                         install: true,
                         extra_files: [space_ouija_headers, space_ouija_extra_files, space_ouija_doc_extra_files]
                        )

python3.install_sources(space_ouija_python_sources, pure: false, subdir: 'space_ouija')
