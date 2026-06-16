string(REGEX REPLACE ".*/(.*)" "\\1" projectName ${CMAKE_CURRENT_SOURCE_DIR})
project(${projectName})



#设置平台
if(BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE "Debug")     #debug   release  
	SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -g -ggdb -Wall -fPIC -shared -lpthread")
	SET(LIBPATH "/../lib/linux_x64/Debug")
	SET(OUTPATH "/../Debug")
else()
    set(CMAKE_BUILD_TYPE "Release")     #debug   release  
	SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -w -fPIC -lpthread")   # -shared
	SET(LIBPATH "/../lib/linux_x64/Release")
 
	SET(OUTPATH "/../Release")
endif()

message("release type:" ${BUILD_TYPE})

if(ARCH_TYPE STREQUAL "aarch64")
    set(ARCH_TYPE "aarch64")    #aarch64  x64
else()
    set(ARCH_TYPE "x64")        #aarch64  x64	 
endif()


if(ARCH_TYPE STREQUAL aarch64)
    message("arch type:aarch64")
	set(CMAKE_SYSTEM_PROCESSOR aarch64)
	SET(CMAKE_CXX_FLAGS "-std=c++14")
else()
    message("arch type:x64")
	SET(CMAKE_CXX_FLAGS "-std=c++14 -mfma ")  #-mavx2
endif()



add_compile_definitions(HC_EXPORT)



# 设置输出目录
SET(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}${OUTPATH})


# 附加库目录和静态库
SET(INC_DIR /usr/local/include)
include_directories(${INC_DIR})
include_directories("include")
include_directories("include/Project-Based/module")
include_directories("include/Project-Based/utils/spdlog/include")

include_directories("SourceCode")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}${LIBPATH}")
link_directories("${LIBRARY_OUTPUT_PATH}")
link_directories("/usr/lib64")
link_directories("/usr/local/lib")
link_directories("/usr/local/lib64")
link_directories("/usr/local/hcdepend/jsoncpp/lib")
link_directories("/usr/local/hcdepend/zlib/lib")



# 忽略的源代码文件
#SET(List_Ignore_Cpp "unzip.cpp" "zip.cpp")
#SET(List_Ignore_Cpp "ProjectAPI.cpp" "ProjectAPI.h" "ProjectCom.cpp" "ProjectCom.h")

file(GLOB_RECURSE FILE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/SourceCode/*.*)
foreach(vFile ${FILE_LIST})
	STRING(REGEX REPLACE ".+/(.+)" "\\1" FileName ${vFile})
	list(FIND List_Ignore_Cpp ${FileName} list_index)
	if(list_index GREATER_EQUAL 0)
		continue()
	endif()
	
    string(REGEX MATCH "SourceCode/.*" simFile ${vFile})
	add_library(${projectName} SHARED ${simFile})
	
	# 筛选器分组
	string(REPLACE "/${FileName}" "" Filter ${simFile})
	string(REPLACE "/" "\\\\" Filter ${Filter})
	# message(STATUS ${Filter})
	source_group(${Filter} FILES ${simFile})
endforeach()

# 引用静态库和动态库
 
target_link_libraries(${projectName} libjsoncpp.a)
target_link_libraries(${projectName} libUtility.a)
target_link_libraries(${projectName} libz.so)
target_link_libraries(${projectName} libProjectBased.so)

# 编译选项
# target_compile_options(${projectName} PRIVATE "/Zi")
# target_compile_options(${projectName} PRIVATE "/sdl")
# target_compile_options(${projectName} PRIVATE "/GL")
# target_compile_options(${projectName} PRIVATE "/GF")
# target_compile_options(${projectName} PRIVATE "/Gy")
# target_compile_options(${projectName} PRIVATE "/permissive-")
# target_compile_options(${projectName} PRIVATE "/FC")