# not upstream: compiles a .rc script into a C++ resource table (rc2cpp.py) and adds it to a target
# orbiter_rc_resources(<target> <file.rc> [SYMBOL <name>] [EXE]) ; default symbol: oapiModuleResources

find_package(Python3 REQUIRED COMPONENTS Interpreter)

# the depfile names absolute paths, which Ninja takes as they are
if(POLICY CMP0116)
	cmake_policy(SET CMP0116 NEW)
endif()

function(orbiter_rc_resources target rcfile)
	cmake_parse_arguments(RC "EXE" "SYMBOL" "" ${ARGN})
	get_filename_component(rcabs ${rcfile} ABSOLUTE)
	get_filename_component(rcname ${rcfile} NAME_WE)
	get_filename_component(rcdir ${rcabs} DIRECTORY)
	set(out ${CMAKE_CURRENT_BINARY_DIR}/${rcname}_rc.cpp)
	set(args)
	if(RC_SYMBOL)
		list(APPEND args --symbol ${RC_SYMBOL})
	endif()
	if(RC_EXE)
		list(APPEND args --exe)
	endif()
	add_custom_command(
		OUTPUT ${out}
		COMMAND ${Python3_EXECUTABLE} ${ORBITER_SOURCE_ROOT_DIR}/cmake/rc2cpp.py ${rcabs} ${out} ${args}
			--depfile ${out}.d -I ${rcdir} -I ${ORBITER_SOURCE_SDK_INCLUDE_DIR}
		DEPENDS ${rcabs} ${ORBITER_SOURCE_ROOT_DIR}/cmake/rc2cpp.py
		DEPFILE ${out}.d
		COMMENT "Compiling resources ${rcname}.rc"
		VERBATIM
	)
	target_sources(${target} PRIVATE ${out})
endfunction()
