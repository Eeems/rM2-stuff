project(yaft)

add_executable(${PROJECT_NAME}
  main.cpp
  parse.c
  fb/common.cpp
  keyboard.cpp)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    rMlib)
