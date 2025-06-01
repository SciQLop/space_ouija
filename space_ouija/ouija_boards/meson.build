fs.copyfile('__init__.py' ,'__init__.py')

boards = {}

subdir('cassini')
subdir('messenger')

# Building all binary modules declared in boards into this directory
subdir('_binary_modules')


# Copying all python files into the right location in the build directory
foreach name, board:boards
    foreach py_file:board['python_sources']
        configure_file(
                             input: py_file,
                             output: name,
                             install_dir: name,
                             copy: true
        )
    endforeach
endforeach
