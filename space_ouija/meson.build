space_ouija_python_sources = [
    '__init__.py',
    'ouija_boards/__init__.py',
    'ouija_boards/cassini/__init__.py',
    'ouija_boards/messenger/__init__.py'
    ]

fs = import('fs')
fs.copyfile('__init__.py' ,'__init__.py')

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
 'py_array.hpp'
)


space_ouija_dep = declare_dependency(
    include_directories : include_directories('.'),
    dependencies : [cpp_utils, fmt_dep],
    extra_files: [space_ouija_headers, space_ouija_extra_files, space_ouija_doc_extra_files]
)

subdir('ouija_boards')

python3.install_sources(files(space_ouija_python_sources), pure: false, subdir: 'space_ouija', preserve_path: true)
