cmake_minimum_required(VERSION 3.20)

# ~ Sources ~ 
file(GLOB_RECURSE TARGET_HEADER_FILES
${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl
)

file(GLOB_RECURSE TARGET_SOURCE_FILES
${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx
${CMAKE_CURRENT_SOURCE_DIR}/src/*.c++
)



# ~ Target ~
set(TARGET_FILES ${TARGET_HEADER_FILES} ${TARGET_SOURCE_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TARGET_FILES})
set(TARGET_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)



# ~ Executable ~
get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
add_executable(${TARGET_NAME})
set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${TARGET_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD_SHIPPING TRUE)

target_sources(${TARGET_NAME} PRIVATE ${TARGET_FILES})

target_include_directories(${TARGET_NAME} PRIVATE ${TARGET_INCLUDE_DIR})
target_include_directories(${TARGET_NAME} PRIVATE ../Maths/include)
target_include_directories(${TARGET_NAME} PRIVATE ${DEPENDS_DIR}/glm/include/)
target_include_directories(${TARGET_NAME} PRIVATE ${DEPENDS_DIR}/catch2/include/)

target_link_libraries(${TARGET_NAME} PRIVATE Maths)
target_link_libraries(${TARGET_NAME} PRIVATE 
	debug ${DEPENDS_DIR}/catch2/lib/Catch2d.lib
	optimized ${DEPENDS_DIR}/catch2/lib/Catch2.lib)
target_link_libraries(${TARGET_NAME} PRIVATE 
	debug ${DEPENDS_DIR}/catch2/lib/Catch2Maind.lib
	optimized ${DEPENDS_DIR}/catch2/lib/Catch2Main.lib)

# ~ Compiler ~
if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
    target_link_options(${TARGET_NAME} PRIVATE /FORCE:UNRESOLVED)
else()
    message(WARNING "This CMakeLists.txt only run on Windows")
endif()
