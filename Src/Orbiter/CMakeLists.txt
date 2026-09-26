# Copyright (c) Martin Schweiger
# Licensed under the MIT License

FetchContent_MakeAvailable(imgui)
FetchContent_MakeAvailable(implot)

# Sources for all Orbiter executable targets
set(common_src
# General source files
	Astro.cpp
	Camera.cpp
	cmdline.cpp
	Config.cpp
	console_ng.cpp
	Element.cpp
	elevmgr.cpp
	Help.cpp
	Input.cpp
	Keymap.cpp
	LightEmitter.cpp
	Mesh.cpp
	Nav.cpp
	Orbiter.cpp
	PlaybackEd.cpp
	Psys.cpp
	Script.cpp
	Shadow.cpp
	State.cpp
	Vecmat.cpp
	VectorMap.cpp
    ConsoleManager.cpp
	TimeData.cpp
# Launchpad
	Launchpad.cpp
	LpadTab.cpp
	TabAbout.cpp
	TabExtra.cpp
	TabModule.cpp
	TabOptions.cpp
	TabScenario.cpp
	TabVideo.cpp
# Dialogs
	CustomControls.cpp
	DialogWin.cpp
	DlgCamera.cpp
	DlgCapture.cpp
	DlgFocus.cpp
	DlgFunction.cpp
	DlgHelp.cpp
	DlgInfo.cpp
	DlgMap.cpp
	DlgMenuCfg.cpp
	DlgMgr.cpp
	DlgOptions.cpp
	DlgRecorder.cpp
	DlgTacc.cpp
	htmlctrl.cpp
	HtmlHelp.cpp
	ResDialog.cpp
	Select.cpp
	OptionsPages.cpp
# Body classes
	Body.cpp
	BodyIntegrator.cpp
	PinesGrav.cpp
	Celbody.cpp
	Planet.cpp
	Rigidbody.cpp
	Star.cpp
# Vessel classes
	FlightRecorder.cpp
	SuperVessel.cpp
	Vessel.cpp
	Vesselbase.cpp
	Vesselstatus.cpp
# Surface base classes
	Base.cpp
	Baseobj.cpp
# Cockpit classes
	Defpanel.cpp
	hud.cpp
	MenuInfoBar.cpp
	Pane.cpp
	Panel.cpp
	Panel2D.cpp
	VCockpit.cpp
# MFD classes
	Mfd.cpp
	MfdAlign.cpp
	MfdComms.cpp
	MfdDocking.cpp
	MfdHsi.cpp
	MfdLanding.cpp
	MfdMap.cpp
	MfdMap_old.cpp
	MfdOrbit.cpp
	MfdSurface.cpp
	MfdSync.cpp
	MfdTransfer.cpp
	MfdUser.cpp
# API implementations
	CamAPI.cpp
	CelSphereAPI.cpp
	DrawAPI.cpp
	GraphicsAPI.cpp
	MFDAPI.cpp
	ModuleAPI.cpp
	OrbiterAPI.cpp
# Graphics utils
	D3d7util.cpp
	D3dmath.cpp
	Di7frame.cpp
# Utils
	Log.cpp
	Memstat.cpp
	Util.cpp
	PathResolve.cpp
	ZTreeMgr.cpp
# ImGui platform backend
	imgui_impl_qt.cpp
)

set(Orbiter_includes
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/Orbitersdk/include
	${CMAKE_SOURCE_DIR}/OVP
	${CMAKE_CURRENT_BINARY_DIR}
)

# Qt Widgets carries the Launchpad and dialogs; libdl the module loading
set(Orbiter_common_libs
	zlib
	Orbitersdk
	DlgCtrl
	Qt6::Widgets
	${CMAKE_DL_LIBS}
)
set(Orbiter_libs
)

set(Orbiter_depends
	Orbitersdk
	DlgCtrl
)

# not upstream: until every Src/Orbiter file is ported, the exe target is OrbiterPort (built on request only)
# and Orbiter is an empty target the modules depend on
if(ORBITER_PORT_COMPLETE)
	set(OrbiterExe Orbiter)
else()
	set(OrbiterExe OrbiterPort)
	add_custom_target(Orbiter)
endif()

# Orbiter executable (graphics server version)
add_executable(${OrbiterExe} ${common_src})

# the dialog templates, bitmaps, icons and strings of Orbiter.rc as a C++ resource table
orbiter_rc_resources(${OrbiterExe} Orbiter.rc EXE SYMBOL OrbiterResources)

set_target_properties(${OrbiterExe}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${ORBITER_BINARY_ROOT_DIR}
	OUTPUT_NAME Orbiter
	COMPILE_DEFINITIONS "NOGRAPHICS"
	FOLDER Core
	AUTOMOC ON
)
if(NOT ORBITER_PORT_COMPLETE)
	set_target_properties(${OrbiterExe} PROPERTIES EXCLUDE_FROM_ALL ON)
endif()

target_include_directories(${OrbiterExe} PUBLIC ${Orbiter_includes} ${TRACY_CLIENT_INCLUDE} ${imgui_SOURCE_DIR}/ ${imgui_SOURCE_DIR}/backends ${implot_SOURCE_DIR}/)

target_link_libraries(${OrbiterExe} ${Orbiter_common_libs} ${Orbiter_libs} ${TRACY_CLIENT})

add_dependencies(${OrbiterExe} ${Orbiter_depends})

# Installation: Orbiter executable (graphics-server versions)
if(ORBITER_PORT_COMPLETE)
	install(TARGETS Orbiter	RUNTIME DESTINATION ${ORBITER_INSTALL_ROOT_DIR})
endif()

# If the standard Orbiter executable is not built...
set_target_properties(${OrbiterExe} PROPERTIES ENABLE_EXPORTS 1)

# no import library on ELF: modules bind the exe's exported oapi* symbols when they are loaded

string(TIMESTAMP DATE "%d %b %Y")
string(TIMESTAMP VERSION "%y%m%d")
configure_file(about.hpp.in about.hpp @ONLY)
