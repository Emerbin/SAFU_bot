cmake_minimum_required(VERSION 3.16.0)
project(cBot)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(Boost_USE_MULTITHREADED ON)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(libmysql REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

set(
    SRC_LIST 
	src/main.cpp 
	src/cBot/cBot.cpp
	src/MessageProcessing/messageTypeParser.cpp 
	src/BlockProcessing/blockCreate.cpp
	src/BlockProcessing/blockView.cpp
	src/BlockProcessing/teamCreate.cpp
    )

set(
    HEADERS_LIST
	src/cBot/cBot.h
	src/MessageProcessing/messageTypeParser.h
	src/MessageProcessing/message.h
	src/BlockProcessing/blockCreate.h
	src/BlockProcessing/blockView.h
	src/BlockProcessing/teamCreate.h
    )
include_directories(${CMAKE_SOURCE_DIR}/src/Reminder)
include_directories(${CMAKE_SOURCE_DIR}/src/Database)
include_directories(${CMAKE_SOURCE_DIR}/src/cBot)
include_directories(${CMAKE_SOURCE_DIR}/src/MessageProcessing)
include_directories(${CMAKE_SOURCE_DIR}/src/BlockProcessing)
include_directories(C:/src/tgbot-cpp/include)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(cBot C:/src/tgbot-cpp/build/Debug/TgBot-cpp.lib ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES} ${libmysql_LIBRARIES})