add_library(stb_image STATIC src/stb_image.cpp include/stb_image/stb_image.h)

target_include_directories(stb_image PUBLIC ${CMAKE_SOURCE_DIR}/vendor/stb_image/include)