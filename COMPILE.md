
PREREQUISITES
=============
Ubuntu/Debian package names; other distros have the same libraries under their own names.

- Build tools: `cmake` (3.26+), `ninja-build`, `g++` (C++20), `git`, `pkg-config`
- Graphics: `libvulkan-dev`, `vulkan-validationlayers`, `glslang-tools` (glslangValidator)
- Windows and dialogs: `qt6-base-dev`
- Sound: `libpipewire-0.3-dev`
- Text: `libfreetype-dev`

Lua 5.1, zlib 1.2.11, Dear ImGui, ImPlot and Tracy are fetched by CMake at the same tags
upstream uses; Catch2 is v3.8.1 (see port-plan.md).

If you want to build the Orbiter documentation (`-DORBITER_MAKE_DOC=ON`), you need LaTeX:
`texlive-latex-extra`, `texlive-fonts-recommended`, `texlive-fonts-extra`, `texlive-science`,
`texlive-plain-generic`, `texlive-font-utils`, `ghostscript`.

To build the code-level documentation, you need [Doxygen](https://www.doxygen.nl/index.html) (`doxygen`, `graphviz`).


BUILDING ORBITER
================
```
cmake --preset linux-x64-release
nice -n 10 cmake --build --preset linux-x64-release
ctest --preset linux-x64-release
```

Binaries and data land in `out/build/linux-x64-release`, in the same layout as upstream
(Modules/, Modules/Plugin/, Modules/Celbody/, Config/, ...).

Other presets: `linux-x64-debug`, `linux-x64-asan`, `linux-x64-tracy`.


PLANETARY TEXTURES
==================
The Orbiter Git repository does not include the planetary texture files for most
celestial bodies. You need to install these separately (e.g., by installing Orbiter
2016 and optionally downloading high-res texture packs from the Orbiter website).

Set the environment variable `ORBITER_PLANET_TEXTURE_INSTALL_DIR` in your profile so that
the Orbiter build correctly configures the reference in the configuration.

Assuming Orbiter 2016 is installed in `~/orbiter2016`:

```
export ORBITER_PLANET_TEXTURE_INSTALL_DIR=~/orbiter2016/Textures
```

Alternatively, you can specify the location of the texture files as a CMake variable:
```
cmake --preset linux-x64-debug -DORBITER_PLANET_TEXTURE_INSTALL_DIR=~/orbiter2016/Textures
```
You can also set the planetary texture directory after building
Orbiter by setting the `PlanetTexDir` entry in `Orbiter.cfg`.


TROUBLESHOOTING
===============
* If you get errors during the build, in particular when building documentation (PDF from
LaTeX sources), try disabling multithreaded build support (limit to a single
thread). Some of the document converters/compilers you are using may not be
thread-safe.

* Running under Xvfb: the NVIDIA driver cannot present there; use lavapipe with
`VK_DRIVER_FILES=/usr/share/vulkan/icd.d/lvp_icd.json`.
