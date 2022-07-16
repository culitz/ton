cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)

find_package(Threads REQUIRED)
find_package(GSL)

include("${CMAKE_SOURCE_DIR}/third-party/rapidjson/rapidjson.cmake") 

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${GFLAGS_INCLUDE_DIR}
    ${BENCHMARK_INCLUDE_DIR}
    ${RAPIDJSON_INCLUDE_DIR}
)

add_library(lite-client-common lite-client-common.cpp lite-client-common.h)
target_link_libraries(lite-client-common PUBLIC tdutils tdactor adnllite tl_api tl_lite_api tl-lite-utils ton_crypto ton_block)

add_executable(
  lite-client 
  lite-client.cpp 
  lite-client.h 
  json_helper.cpp 
  json_helper.h
  serializers/fill-utils.h
  serializers/fill-utils.cpp
  serializers/serializer.h
  serializers/transactions_serializer.h
  serializers/transactions_serializer.cpp
  serializers/account_serializer.h
  serializers/account_serializer.cpp
  serializers/block_id_serializer.h
  serializers/block_id_serializer.cpp
)

add_dependencies(lite-client rapidjson)
target_link_libraries(
  lite-client 
  tdutils 
  tdactor 
  adnllite 
  tl_api tl_lite_api 
  tl-lite-utils 
  ton_crypto ton_block
  terminal 
  lite-client-common 
  git 
)

install(TARGETS lite-client RUNTIME DESTINATION bin)
