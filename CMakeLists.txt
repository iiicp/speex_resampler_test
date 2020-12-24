cmake_minimum_required(VERSION 3.17)

project(speex_resampler_test)

#message(STATUS ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

# cflags
LIST(APPEND MY_COMPILE_FLAGS -DFLOATING_POINT -DOUTSIDE_SPEEX -DUSE_SSE)
# dflags
#target_link_libraries(${PROJECT_NAME} PUBLIC mydsp ${MY_LINK_FLAGS})

add_executable(speex_resampler_test main.cpp resample.c)

# 添加 cflags
target_compile_definitions(${PROJECT_NAME} PUBLIC ${MY_COMPILE_FLAGS})

