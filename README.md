[![GitHub License](https://img.shields.io/github/license/SciQLop/space_ouija)](https://mit-license.org/)
[![Documentation Status](https://readthedocs.org/projects/space_ouija/badge/?version=latest)](https://space_ouija.readthedocs.io/en/latest/?badge=latest)
[![CPP17](https://img.shields.io/badge/Language-C++17-blue.svg)]()
[![PyPi](https://img.shields.io/pypi/v/space_ouija.svg)](https://pypi.python.org/pypi/space_ouija)
[![Coverage](https://codecov.io/gh/SciQLop/space_ouija/coverage.svg?branch=main)](https://codecov.io/gh/SciQLop/space_ouija/branch/main)
[![Discover on MyBinder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/SciQLop/space_ouija/main?labpath=examples/notebooks)

# Python packages

| Linux x86_64 | Windows x86_64  | MacOs x86_64  | MacOs ARM64  |
| --- | --- | --- | --- |
| [![linux_x86_64][1]][2] | [![windows_x86_64][3]][4] | [![macos_x86_64][5]][6] | [![macos_arm64][7]][8] |

[1]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml/badge.svg?event=release
[2]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml
[3]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml/badge.svg?event=release
[4]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml
[5]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml/badge.svg?event=release
[6]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml
[7]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml/badge.svg?event=release
[8]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml


# Unit Tests

| Linux x86_64  | Windows x86_64 | MacOs x86_64  |
| --- | --- | --- |
| [![linux_x86_64][9]][10] | [![windows_x86_64][11]][12] | [![macos_x86_64][13]][14] |

[9]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml/badge.svg?event=push
[10]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml
[11]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml/badge.svg?event=push
[12]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml
[13]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml/badge.svg?event=push
[14]: https://github.com/SciQLop/space_ouija/actions/workflows/CI.yml


# space_ouija
Space Ouija is C++ and Python toolkit allowing to read old space missions data 
files using deprecated formats using modern tools.

# Installing

## From PyPi

```bash
python3 -m pip install --user space_ouija
```

## From sources

```bash
meson build
cd build
ninja
sudo ninja install
```

Or if youl want to build a Python wheel:

```bash
python -m build . 
# resulting wheel will be located into dist folder
```

# Basic usage

## Python

### Reading files
Basic example from a local file:

```python
import space_ouija
```
