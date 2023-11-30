# set the project name
get_filename_component(CURRENT_FOLDER_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
set(PROJECT_NAME ${CURRENT_FOLDER_NAME})
set(SOURCE_FOLDE "src")
set(INCLUDE_FOLDE "include")

###############################
#                             #
# Sources                     #
#                             #
###############################

# Add source files
set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FOLDE})
file(GLOB_RECURSE SOURCE_FILES 
	${PROJECT_SOURCE_DIR}/*.c
	${PROJECT_SOURCE_DIR}/*.cc
	${PROJECT_SOURCE_DIR}/*.cpp
	${PROJECT_SOURCE_DIR}/*.cxx
	${PROJECT_SOURCE_DIR}/*.c++)
	
# Add header files
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${INCLUDE_FOLDE})
file(GLOB_RECURSE HEADER_FILES 
	${PROJECT_INCLUDE_DIR}/*.h
	${PROJECT_INCLUDE_DIR}/*.hpp
	${PROJECT_INCLUDE_DIR}/*.inl)
	
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES} ${SOURCE_FILES})

###############################
#                             #
# Library                  	  #
#                             #
###############################

add_library(${PROJECT_NAME} STATIC)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES} ${HEADER_FILES})
#set_target_properties(${TARGET_NAME} PROPERTIES EXCLUDE_FROM_ALL TRUE)
include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/${INCLUDE_FOLDE})

foreach( project_i ${THIRD_PARTY_PROJECT})
	include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/${project_i}/${INCLUDE_FOLDE})
endforeach( project_i )

foreach( include_i ${THIRD_PARTY_INCLUDE})
	include_directories(${PROJECT_NAME} PRIVATE ${DEPENDS_DIR}/${include_i}/${INCLUDE_FOLDE})
endforeach( include_i )

#Qt6
file(GLOB_RECURSE QT_LIB ${PROJECT_NAME} PUBLIC ${DEPENDS_DIR}/Qt6/lib/release/*.lib)
target_link_libraries(${PROJECT_NAME} ${QT_LIB})

#Qt advenced docking
target_link_libraries(${PROJECT_NAME} ${DEPENDS_DIR}/Qtadvanceddocking/lib/qtadvanceddocking.lib)

#Qt Nodes
target_link_libraries(${PROJECT_NAME} 
						debug ${DEPENDS_DIR}/QTNodes/lib/debug/QtNodesd.lib
						optimized ${DEPENDS_DIR}/QTNodes/lib/release/QtNodes.lib)