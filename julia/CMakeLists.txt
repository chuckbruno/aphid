cmake_minimum_required (VERSION 2.8)
project (btree)
 
include_directories ("." "../shared" "../btree")

add_executable(julia Parameter.cpp 
QuatJulia.cpp
main.cpp)

if(WIN32)
	set (Boost_INCLUDE_DIR "D:/usr/boost_1_51_0")
	set (Boost_LIBRARY_DIR "D:/usr/boost_1_51_0/stage/lib")
	set (Boost_USE_STATIC_LIBS ON)
	set (Boost_USE_MULTITHREADED ON)
	include_directories ("D:/usr/boost_1_51_0")
	find_package(Boost 1.51 COMPONENTS system filesystem date_time regex)
#elseif(UNIX)
#	set (Boost_INCLUDE_DIR "/home/OF3D/zhangjian/Documents/boost_1_51_0")
#	set (Boost_USE_STATIC_LIBS ON)
#	set (Boost_USE_MULTITHREADED ON)
#	include_directories ("/home/OF3D/zhangjian/Documents/boost_1_51_0")
#	find_package(Boost 1.51 COMPONENTS system filesystem date_time regex)
else()
	set (Boost_INCLUDE_DIR "/users/jianzhang/Library/boost_1_55_0")
	set (Boost_LIBRARY_DIR "/users/jianzhang/Library/boost_1_55_0/stage/lib")
	include_directories ("/users/jianzhang/Library/boost_1_55_0")
	find_package(Boost 1.55 COMPONENTS system filesystem date_time regex)
	include_directories ("/usr/local/include")
endif()

message ("boost system is" ${Boost_SYSTEM_LIBRARY})
message ("boost date_time is" ${Boost_DATE_TIME_LIBRARY})
message ("boost regex is" ${Boost_REGEX_LIBRARY})
message ("boost filesystem is" ${Boost_FILESYSTEM_LIBRARY})

target_link_libraries (julia ${Boost_SYSTEM_LIBRARY})
target_link_libraries (julia ${Boost_FILESYSTEM_LIBRARY})
target_link_libraries (julia ${Boost_DATE_TIME_LIBRARY})
target_link_libraries (julia ${Boost_REGEX_LIBRARY})

if (WIN32)
set (Aphid_Library "D:/aphid/lib/aphid.lib")
else ()
set (Aphid_Library "/users/jianzhang/aphid/lib/libaphid.a")
endif ()

target_link_libraries (julia ${Aphid_Library})
