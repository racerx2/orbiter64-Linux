# Copyright (c) Martin Schweiger
# Licensed under the MIT License

add_library(TrackIR SHARED
	TrackIR.cpp
	TrackIRconfig.cpp
	NPClientWraps.cpp
)
orbiter_rc_resources(TrackIR TrackIR.rc)

target_include_directories(TrackIR
	PUBLIC ${ORBITER_SOURCE_SDK_INCLUDE_DIR}
	PUBLIC ${MODULE_COMMON_DIR}
)

target_link_libraries(TrackIR
	${ORBITER_LIB}
	${ORBITER_SDK_LIB}
	UxTheme.lib
)

add_dependencies(TrackIR
	${OrbiterTgt}
	Orbitersdk
)

set_target_properties(TrackIR
	PROPERTIES
	FOLDER Modules
)

# Installation
install(TARGETS TrackIR
	LIBRARY
	DESTINATION ${ORBITER_INSTALL_PLUGIN_DIR}
)
