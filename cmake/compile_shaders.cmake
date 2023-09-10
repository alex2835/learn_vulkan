####################################################################################################
# This function compile any GLSL shader into SPIR-V shader and embed it in a C header file.
# Example:
#	compile_shaders(TARGETS "assets/shader/basic.frag" "assets/shader/basic.vert")
####################################################################################################

function(compile_shaders TARGET_NAME)

	include(CMakeParseArguments)
    cmake_parse_arguments(SHADERS "" "" "TARGETS" ${ARGN})

	# Note: if it remains unparsed arguments, here, they can be found in variable PARSED_ARGS_UNPARSED_ARGUMENTS
	if(NOT SHADERS)
		message(FATAL_ERROR "You must provide targets.")
	endif()

	# Find the glslangValidator executable
	set(glslCompiler "glslangValidator")

	# Cache dir
	set(TEMP_DIR "${PROJECT_SOURCE_DIR}/temp/shaders")
	file(MAKE_DIRECTORY ${TEMP_DIR})

	# For each shader, we create a header file
	foreach(SHADER ${SHADERS})

		# Prepare a header name and a global variable for this shader
		get_filename_component(SHADER_NAME ${SHADER} NAME)
		string(REPLACE "." "_" HEADER_NAME ${SHADER_NAME})
		string(TOUPPER ${HEADER_NAME} GLOBAL_SHADER_VAR)
		
		set(SHADER_SPV "${TEMP_DIR}/${SHADER_NAME}.spv")
		#set(SHADER_SPV "${SHADER}.spv")
		set(SHADER_HEADER "${TEMP_DIR}/${HEADER_NAME}.h")

		# Create header file with binary const
		execute_process(COMMAND ${glslCompiler} -V ${SHADER} -o ${SHADER_SPV})
		embed_resource(${SHADER_SPV} ${SHADER_HEADER} ${GLOBAL_SHADER_VAR})
		
		message(STATUS "Generating build commands for ${SHADER}")
	endforeach()

	target_include_directories( ${TARGET_NAME} PUBLIC ${TEMP_DIR})

endfunction()