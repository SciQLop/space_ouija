project(
  'space_ouija',
  'cpp','c',
  meson_version: '>= 1.2.0',
  version : run_command('scripts/version.py',
                        capture:true,
                        env:{'SRC_ROOT':meson.project_source_root()}
                        ).stdout().strip() ,
  default_options : ['warning_level=3', 'cpp_std=c++20', 'default_library=static', 'buildtype=release'],
  license : 'GPL3'
)

if get_option('buildtype').contains('debug')
    add_project_arguments('-DSPACE_OUIJA_ENABLE_ASSERT', language : ['cpp'])
    add_project_arguments('-DSPACE_OUIJA_HEDLEY', language : ['cpp'])
endif

cpp = meson.get_compiler('cpp')
if('clang'==cpp.get_id())
    add_project_arguments('-fsized-deallocation', language : 'cpp')
endif

pybind11_dep = dependency('pybind11')
hedley_dep = dependency('hedley')
fmt_dep = dependency('fmt')
cpp_utils = dependency('cpp_utils', required : true,  default_options : ['cpp_std=c++20'])



if build_machine.system() == 'windows'
    link_args = ['-static-libstdc++','-static-libgcc','-static']
elif build_machine.system() == 'darwin' and cpp.get_id() != 'clang'
    link_args = ['-static-libstdc++','-static-libgcc']
else
    link_args = []
endif

space_ouija_extra_files = files(
    'pyproject.toml',
    'README.md',
    '.bumpversion.cfg',
    'version.txt',
    '.github/workflows/CI.yml',
    '.github/workflows/tests-with-coverage.yml'
)

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

subdir('space_ouija')

summary({
  'C': meson.get_compiler('c').cmd_array(),
  'C++': meson.get_compiler('cpp').cmd_array()
        }, section: 'Compilers')
