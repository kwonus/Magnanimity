cmake_minimum_required (VERSION 3.19)
project ("Magnanimity")

include_directories(BEFORE "./")
include_directories(AFTER "C:/Users/kevin/source/C/XVMem")
link_directories("C:/Users/kevin/source/C/XVMem/out/build/x64-Debug")
find_library(XVMEM_LIB XVMem)
link_libraries(XVMem)

# Add source to this project's executable.
add_executable (Magnanimity "Magnanimity.cpp" "Magnanimity.h" "md5.cpp" "md5.h")

