project(
  'space_ouija',
  'cpp','c',
  meson_version: '>= 0.64.0',
  version : run_command('scripts/version.py',
                        capture:true,
                        env:{'SRC_ROOT':meson.project_source_root()}
                        ).stdout().strip() ,
  default_options : ['warning_level=3', 'cpp_std=c++17', 'default_library=static', 'buildtype=release'],
  license : 'GPL3'
)

if get_option('buildtype').contains('debug')
    add_project_arguments('-DSPACE_OUIJA_ENABLE_ASSERT', language : ['cpp'])
    add_project_arguments('-DSPACE_OUIJA_HEDLEY', language : ['cpp'])
endif


conf_data = configuration_data()
conf_data.set_quoted('-DSPACE_OUIJA_VERSION', meson.project_version())


pybind11_dep = dependency('pybind11')
hedley_dep = dependency('hedley')
fmt_dep = dependency('fmt')

if build_machine.system() == 'windows'
    link_args = ['-static-libstdc++','-static-libgcc','-static']
elif build_machine.system() == 'darwin' and cpp.get_id() != 'clang'
    link_args = ['-static-libstdc++','-static-libgcc']
else
    link_args = []
endif


space_ouija_doc_extra_files =  files(
  'docs/conf.py',
  'docs/Makefile',
  'docs/requirements.txt',
  'docs/index.rst',
  'docs/authors.rst',
  'docs/contributing.rst',
  'docs/history.rst',
  'docs/installation.rst',
  'docs/examples/index.rst'
)


summary({
  'C': meson.get_compiler('c').cmd_array(),
  'C++': meson.get_compiler('cpp').cmd_array()
        }, section: 'Compilers')
