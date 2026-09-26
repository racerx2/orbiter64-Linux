# Orbiter → Linux native port (Orbiter64-linux)

## Working rules (standing — apply every session)

1. **Go page by page, line by line.** Port from the actual upstream file, top to bottom. Each upstream
   file is converted in place (same path, same classes and functions, same order, upstream's own short
   comments kept), so `git diff upstream/main -- <file>` is the port of that file.
2. **No wrappers. Pure Linux.** No Wine, DXVK or Win32 emulation layer. Every Win32/DirectX call gets a
   real Linux/Vulkan implementation of the same behaviour; missing counterparts are built, not stubbed.
3. **Do not use long form comments.** Our comments are one-liners, no banners. Upstream's own comments
   (Doxygen included) stay untouched so diffs and upstream merges only show real changes.
4. **Nothing dropped silently.** Where something truly can't apply, a one-line comment at that spot
   says what is left out and why. Anything upstream doesn't have is marked `not upstream: <why>`.
5. **Don't use grep** (Desktop Commander refuses it). Use awk.
6. **Keep the machine load low.** `nice -n 10`, at most 3 jobs, one heavy job at a time, never
   builds and tests together.
7. **Never delete files.** `mv -n` into `~/Projects/_to_delete/`. Don't change system settings;
   leave the user's config folders alone — tests use sandbox copies.
8. **Verify before claiming.** Test it, then state it.
9. **Keep up with upstream.** At the start of each session and each phase: `git fetch upstream`,
   `git merge upstream/main`, port every changed file line by line, advance `UPSTREAM`, log it under
   Upstream sync. `~/orbiter` stays untouched.
10. **Other Linux forks of Orbiter (OpenGL/SDL3) are not used for anything.**

**Stack:** C++20 · CMake + Ninja · GCC 15 · Vulkan 1.4 core · Qt 6 Widgets · PipeWire · Lua 5.1.
**Upstream:** orbitersim/orbiter, MIT (D3D9Client and its port are LGPL). Reference checkout `~/orbiter`.
**Repo:** github.com/racerx2/orbiter64-Linux (fork of upstream), cloned at `~/Projects/Orbiter64-linux`.
Remotes: `origin` = the fork, `upstream` = orbitersim/orbiter. Port commits sit on top of upstream `main`.
Commits are authored as racerx2 with the GitHub noreply address.

## Decisions (2026-09-26)

| Topic | Decision |
|---|---|
| Windows and dialogs | Qt 6 Widgets. Each `.rc` DIALOG → Qt dialog with the same controls, IDs and layout. Render window = QWindow + Vulkan surface. |
| Graphics | `OVP/D3D9Client` → `OVP/VulkanClient`, file for file. Vulkan 1.4 core: dynamic rendering, sync2, timeline semaphores, descriptor indexing, push descriptors. Validation layers in debug. |
| Shaders | The 23 HLSL/.fx files → GLSL 460 line by line, SPIR-V at build time via glslangValidator. |
| Sound | XRSound engine reimplemented on PipeWire (irrKlang is closed-source). |
| Data | Copied like upstream CopyData, everything included, from the repo's own data folders into upstream's build layout. |
| SDK Win32 types | Integer types keep their names (DWORD, UINT, BOOL, WORD, BYTE, COLORREF as fixed-width typedefs). Handle types (HWND, HINSTANCE, HDC, HFONT, HBITMAP, DLGPROC, WPARAM, LPARAM) become real Linux types; the APIs that use them are redone. |
| Git | Port commits on top of upstream `main` in the fork; upstream syncs by merge. |

## Build

```
cmake --preset linux-x64-release
nice -n 10 cmake --build --preset linux-x64-release      # jobs: 3 in the preset
ctest --preset linux-x64-release
```
Output: `out/build/linux-x64-release/`, upstream layout. Background builds log to `/tmp/ob_build.log`.

## Upstream sync

| Date | From | To | Upstream commits | Port changes |
|---|---|---|---|---|
| 2026-09-26 | — | `4137930c` | base (main HEAD, PR #679) | — |
| 2026-09-26 | `4137930c` | `4137930c` | none new (Phase 1 close) | — |

## Phases

| # | Phase | Status |
|---|---|---|
| 0 | Setup: CMake skeleton, Extern, test harness, port-plan | done 2026-09-26 |
| 1 | SDK + core utils (Orbitersdk, Vecmat, Astro, Element, Log, Util, Config, file resolver) | done 2026-09-26 |
| 2 | Celbody modules + Body/Planet/Star/Psys/BodyIntegrator/PinesGrav, headless run | — |
| 3 | Vessel core + `.so` module loading, ShuttlePB first | — |
| 4 | Launchpad (Launchpad, Tab*, OptionsPages, Orbiter.rc dialogs) | — |
| 5 | VulkanClient, first light: stars + planet | — |
| 6 | In-sim ImGui dialogs, input, keymaps | — |
| 7 | Vessels, plugins, LuaScript, XRSound | — |
| 8 | Utils, packaging (tar.gz + launcher), help, docs | — |

## Layer map (upstream → port)

| Upstream | Status |
|---|---|
| `CMakeLists.txt`, `CMakePresets.json`, `cmake/tracy.cmake`, `cmake/sanitizer.cmake`, `cmake/CPackOptions.cmake.in` | ported (Phase 0) |
| `Extern/CMakeLists.txt`, `Extern/Lua`, `Extern/zlib`, `Extern/imgui/imconfig.h` | ported (Phase 0) |
| `Extern/imgui/CMakeLists.txt`, `cmake/cpack_install.cmake`, Penlight, ldoc, luafilesystem | unchanged, work as is |
| `Tests/` | harness ported; `Lua.Interpreter` waits for Phase 7 |
| `README.md`, `COMPILE.md` | ported to Linux |
| `readme.txt` | Phase 8 |
| `Extern/Htmlhelp`, `compileOrbiter.py`, `cmake/FindDXSDK.cmake`, `cmake/*.bat.in` | left in the tree, unused: Windows only |
| `Src/Orbiter/Vecmat.h/.cpp` | ported (Phase 1), unit-tested |
| `Src/Orbiter/Astro`, `TimeData` | ported (Phase 1), unit-tested |
| `Src/Orbiter/PathResolve.cpp` | not upstream: `oapiResolvePath`, case-insensitive and `\`-tolerant file lookup, unit-tested |
| `Src/Orbiter/Log`, `Memstat` | ported (Phase 1) |
| `Src/Orbiter/Di7frame`, `Input` | ported (Phase 1): DirectInput 7 → Qt key events + evdev joysticks, unit-tested |
| `Src/Orbiter/Keymap`, `Element`, `Util`, `Select`, `Orbiter.h`, `Mesh.h` | ported (Phase 1), syntax-checked with g++ 15 |
| `Src/Orbiter/Config`, `State`, `cmdline` | converted (Phase 1); full compile once VectorMap/Vessel/Launchpad headers are in (Phases 2–4) |
| `Orbitersdk/include/*.h` (OrbiterAPI, GraphicsAPI, DrawAPI, ModuleAPI, CelBodyAPI, VesselAPI, MFDAPI, CamAPI, CelSphereAPI, Orbitersdk) | ported (Phase 1), all compile with g++ 15 |
| `Orbitersdk/include/OrbiterPlatform.h` | not upstream: the windows.h types the SDK uses |
| `Orbitersdk/include/DlgCtrl.h`, `afxres.h` | Phase 4 (dialog controls, .rc support) |
| `Src/Orbitersdk/Orbitersdk.cpp` + CMake | ported (Phase 1): builds `libOrbitersdk.a` |
| `Orbitersdk/CMakeLists.txt` | ported; `samples`, `sample_build` not yet |
| rest of `Src/`, `Orbitersdk/`, `OVP/`, `Sound/`, `Utils/`, `Html/`, `Doc/` | not started (not in the build yet) |

## Deviations (not upstream)

- C++20 instead of C++17.
- CopyData uses `copy_directory_if_different` so the 228 MB of textures isn't re-copied every build.
- Option `ORBITER_BUILD_D3D9CLIENT` → `ORBITER_BUILD_VULKANCLIENT`; DXSDK lookup → `find_package(Vulkan 1.4 COMPONENTS glslangValidator)`.
- `IRRKLANG_DIR` → `pkg_check_modules(PIPEWIRE libpipewire-0.3)`.
- Qt5_x64_DIR → `find_package(Qt6 Core Gui Widgets)`.
- `CMAKE_INSTALL_RPATH $ORIGIN`: Windows loads a DLL's dependencies from the exe folder.
- `ORBITER_SANITIZER` passes `address` (upstream passed the ON/OFF value as the sanitizer name, MSVC only).
- CPack: TGZ instead of WIX/ZIP; WIX banner/icon/GUID left out.
- Lua: `LUA_USE_POSIX LUA_USE_DLOPEN` + `-ldl` stand in for `LUA_BUILD_AS_DLL`.
- lfs: built as `lfs.so` (no `lib` prefix) so `require "lfs"` finds it; `/DEF:lfs.def` not needed on ELF.
- zlib 1.2.11: `CMAKE_POLICY_VERSION_MINIMUM 3.5`, since CMake 4 rejects its `cmake_minimum_required(2.4.4)`.
- imconfig.h: `__declspec(dllexport/dllimport)` on `GImGui`/`GImPlot` → default visibility in the exe, plain `extern` in modules.
- Catch2 `v3.0.0-preview3` → `v3.8.1`: preview3 doesn't compile on glibc 2.34+ (non-constant `MINSIGSTKSZ`).
  Built static in Tests, since tracy.cmake makes everything shared.
- Tests: `add_unit_test()` for ported code that runs without the Orbiter exe; `Port.Harness` smoke test.
- Presets: `linux-gcc-base` replaces the Windows and winegcc presets; build presets use `jobs: 3`;
  the asan preset sets `ORBITER_SANITIZER` (upstream's `ORBITER_ENABLE_ASAN` doesn't exist).

### SDK (Phase 1)
- `OrbiterPlatform.h` replaces `<windows.h>` in the SDK: fixed-width DWORD/WORD/BYTE/UINT/BOOL/LONG/INT16/COLORREF
  and the *_PTR ints, RECT/POINT/SIZE (+LP*), RGB/Get*Value, `MAX_PATH` = 4096, Qt forward declarations.
- Handle types: HWND → `QWidget*` (dialogs) / `QWindow*` (render window), HINSTANCE → `void*` dlopen handle,
  HDC → `QPainter*`, HFONT → `QFont*`, HPEN → `QPen*`, HBITMAP → `QImage*`, window messages → `QEvent*`.
- DLGPROC → `DLGINIT (QWidget *hDlg, void *context)`: called once after the dialog is built from its resource;
  the module connects its controls' Qt signals. `oapiDefDialogProc` left out (oapiOpenDialog wires defaults).
- MFD message procs: WPARAM/LPARAM → `uintptr_t`/`intptr_t`, `OAPI_MSGTYPE` → `intptr_t`.
- `GraphicsClient::RenderWndProc (QWindow*, QEvent*)` → bool, `LaunchpadVideoWndProc (QWidget*)` called once
  per video tab — provisional until Phases 4/5 port their callers. WIC factory left out (QImage does images).
- DLLEXPORT/DLLCLBK → `__attribute__((visibility("default")))`; DLLIMPORT empty.
- `#include <lua/lua.h>` → `<Lua/lua.h>` (the folder is `Lua`).
- `oapiWriteLogError` uses `__VA_OPT__(,)` (MSVC drops the empty comma itself).
- `__declspec(align(16))` → `alignas(16)`; `<algorithm>` included unconditionally (was MSVC<2019 only).
- IVECTOR2 `long` → `LONG`: Windows long is 32-bit, LP64 long is 64-bit. General rule for all later files:
  every `long` that touches a file format or a struct layout gets checked.
- FMATRIX4's `_x/_y/_z/_p` row view: g++ allows no members with constructors in an anonymous struct, so the rows
  are `FVECTOR4_T`, a trivial same-layout twin with conversions to/from FVECTOR4 (`.xyz` via `FVECTOR3_T`).
- `class ATMOSPHERE;` / `class Instrument_User;` forward declarations: MSVC lets a friend declaration introduce
  the name, g++ doesn't.
- `Orbitersdk.cpp`: DllMain → ELF constructor/destructor; the module finds its own handle with
  `dladdr` + `dlopen(RTLD_NOLOAD)` and skips the exe (which links the same static lib). Verify with the first
  real module in Phase 2.
- `libOrbitersdk.a` is built `-fPIC` (it is linked into .so modules).
- `PSTR`/`PCSTR`, `DWORDLONG`, `LONGLONG`, `INT`, `FLOAT`, `VOID` added to OrbiterPlatform.h as the core files need them.

### Core (Phase 1)
- Handles follow the SDK map; `HRESULT` in Orbiter's own methods → `int` (0 = success).
- Direct3D 7 data types in core files: D3DVALUE → float, D3DVECTOR → `oapi::FVECTOR3`, D3DMATRIX → `oapi::FMATRIX4`,
  D3DMATERIAL7 → MATERIAL, D3DCOLORVALUE → COLOUR4, D3DVERTEX → NTVERTEX, D3DCOLOR → DWORD. The inline D3D7 render
  path (`Mesh::Render/RenderGroup/MakeGroupVertexBuffer`, LPDIRECT3DDEVICE7/LPDIRECTDRAWSURFACE7) is dead upstream
  (the graphics client renders) → left out with one-line comments.
- Every file open goes through `oapiResolvePath`; `_stricmp`/`_strnicmp`/`stricmp` → `strcasecmp`/`strncasecmp`.
- CRLF data files: Linux streams keep `'\r'`, so `trim_string` and `Config::GetString` drop it.
- `va_list` is `va_copy`'d before a second use (x86-64 SysV consumes it).
- Log: QueryPerformanceCounter → `steady_clock`, GetLastError → `errno`/`strerror`, module list via
  `dl_iterate_phdr`, DebugBreak → `raise(SIGTRAP)`; `LogOut_DDErr`/`LOGOUT_DDERR` left out (DirectDraw).
- Memstat: working set → resident pages from `/proc/self/statm`.
- Input: keyboard fed from Qt key events (`nativeScanCode()-8` = evdev code → DIK code; 1–88 identical, extended
  keys by table) with DI-style state array and 10-entry buffer; joysticks read from `/dev/input/event*` with DI
  range/deadzone/saturation, axes X,Y,Z,RX,RY,RZ, slider0 = throttle, slider1 = rudder, hats → POV, buttons in
  code order. DIJOYSTATE2 → `JoyState`, DIDEVICEOBJECTDATA → `KeyData`. DI mouse device left out (Qt mouse events).
- Config: display size = primary `QScreen` size × devicePixelRatio; scenario paths absolute when they start with `/`.
- Util: `MakePath` creates each missing level with `mkdir` (SHCreateDirectoryEx); `Get/SetClientPos` on QWidget.
- cmdline: console attach left out (stdout is the launching terminal); help names `Orbiter` and `Modules/Plugin/<pg>.so`.
- Modules are `<name>.so` without a `lib` prefix, matching upstream's `<name>.dll` lookups.
- `class ScriptInterface;` forward declaration in Orbiter.h (friend rule, as in the SDK).
- Sources are UTF-8 with a few cp1252 bytes (e.g. Element.cpp); edits keep the bytes as they are.

## Left out (can't apply)

- 8 moon Celbody modules that exist only as 32-bit DLLs (Ariel, Deimos, Miranda, Oberon, Phobos, Titania,
  Triton, Umbriel) — Kepler fallback, as upstream HEAD does.
- BinAssets `msvc*71.dll`, `InterfaceBuilder.exe`, `DxTex.exe`, tileedit's Windows libpng/zlib — binary only.
  (BinAssets is still copied as the data decision says.)
- hhc_fix.bat / pdftex_fix.bat, FindDXSDK.cmake, `/MP`, `/permissive`, `/we4311`, `NOMINMAX`, `/LARGEADDRESSAWARE`.

## Log

### 2026-09-26 — Phase 0
- Upstream checked: `main` = `4137930c` (same as `~/orbiter`), nothing to sync.
- Root CMake, presets, cmake/, Extern/, Tests/ ported; README/COMPILE rewritten for Linux.
- Catch2 preview3 build failure found and fixed (see Deviations).
- Verified: configure + build clean (only zlib 1.2.11 K&R warnings), `ctest` 1/1 passed,
  `./lua` loads `lfs.so` and Penlight, CopyData put 228 MB Textures + Config etc. in the build tree.
- Moved onto the fork: the first standalone repo (commit 083287c) was replaced by a clone of
  racerx2/orbiter64-Linux with Phase 0 replayed on top of upstream `main`; the old folder is in
  `~/Projects/_to_delete/`. Data now comes from the repo like upstream (the `ORBITER_DATA_SOURCE_DIR`
  variable is gone).

### 2026-09-26 — Phase 1 start
- SDK types decision taken (see Decisions).
- `Src/Orbiter/Vecmat.h/.cpp`: read top to bottom; only change is the `qrdcmp` default argument, which g++
  rejects on a friend declaration — moved to a namespace-scope declaration before `Matrix`/`Matrix4`.
  `asinh`/`acosh` inlines compile as is against glibc. `Tests/Vecmat.Test.cpp` (vectors, matrix inverse,
  quaternion round trip, 3x3/4x4 QR solves, plane helpers) passes.

### 2026-09-26 — Phase 1 done
- `b8fe15db` Astro, TimeData, `oapiResolvePath` (+ Astro.Test, PathResolve.Test).
- `28ca6d4d` + `5a34bf64` Log, Memstat, DirectInput layer on evdev (+ Input.Test), Config (28ca6d4d's message
  named Config before it was applied; 5a34bf64 carries it).
- `759f531d` Keymap, Mesh.h, Orbiter.h, Util, Element, State, cmdline.
- Verified: build clean, `ctest` 5/5 passed; upstream checked, no new commits.

## Bugs

- Upstream root CMake points `ldoc` at `packages/LDoc/ldoc.lua` but CopyLDoc writes `packages/ldoc`;
  only works on a case-insensitive disk. Port uses `packages/ldoc`.
- `lfs.so` couldn't find `liblua.so` (no RUNPATH) — fixed with `$ORIGIN`.
- `Port.Harness` couldn't find `libCatch2Main.so` — fixed by building Catch2 static.
- Upstream: `FMATRIX4()` writes `m11 = m12 = m13, m14 = ... = 0`, so m11/m12 copy an uninitialised m13 and m13
  is never set. Kept as upstream; g++ warns.
- Upstream State.cpp passes `&color` (`char (*)[256]`) to `sscanf "%255s"`; same address, works. Kept; g++ warns.
