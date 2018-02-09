cmake_minimum_required(VERSION 3.9)
project(DecoAutoGen)

include_directories(/media/juhyun/hdd/extern/llvm-5.0.1.src/tools/clang/include)
include_directories(/media/juhyun/hdd/extern/llvm-5.0.1.src/include)
include_directories(/media/juhyun/hdd/extern/llvm-5.0.1.src/build/include)
include_directories(/media/juhyun/hdd/extern/llvm-5.0.1.src/build/tools/clang/include)

add_compile_options(-fno-rtti)

add_definitions(
        -D__STDC_LIMIT_MACROS
        -D__STDC_CONSTANT_MACROS
        -DGTEST_HAS_RTTI=0
        -DLLVM_BUILD_GLOBAL_ISEL
        -D_GNU_SOURCE
        -D__STDC_FORMAT_MACROS
)

set(CMAKE_CXX_STANDARD 11)

link_directories(/media/juhyun/hdd/extern/llvm-5.0.1.src/build/lib)

add_executable(DecoAutoGen
        DecoAutoGen.cpp test.cpp)

target_link_libraries( DecoAutoGen
        LLVMOption
        clangDriver
        clangFrontend
        clangTooling
        LLVMOption
        clangParse
        LLVMMCParser
        clangSerialization
        clangSema
        clangEdit
        clangAnalysis
        LLVMBitReader
        LLVMProfileData
        clangASTMatchers
        clangFormat
        clangToolingCore
        clangAST
        clangRewrite
        clangLex
        clangBasic
        LLVMCore
        LLVMBinaryFormat
        LLVMMC
        LLVMSupport
)

target_link_libraries( DecoAutoGen
        tinfo
        pthread
        z
)