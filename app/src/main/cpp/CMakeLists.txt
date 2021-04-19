# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2) #最低支持CMake版本

# Declares and names the project.

project("ndk")


#TODO 第一步：导入头文件   ../inc
include_directories("inc")
#批量导入所有源文件
file(GLOB allCPP *.c *.h *.cpp)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        native-lib #TODO 生成libnative-lib.so

        # Sets the library as a shared library.
        SHARED #动态库  .so
        #STATIC #静态库  .a

        # Provides a relative path to your source file(s).
        # native-lib.cpp test.cpp # 源文件
        ${allCPP}
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

#CMAKE_SOURCE_DIR 表示当前CMakeList.txt所在路径
#CMAKE_ANDROID_ARCH_ABI 当前的CPU架构
#TODO 第二部：导入库文件
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}")

find_library( # Sets the name of the path variable.
        log-lib #赋值给
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log) #寻找 /platforms/android-23/arch-arm64/usr/lib/log-lib.so 库

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.


#TODO 第三步：链接具体库，到lib native-lib.so总库
target_link_libraries( # Specifies the target library.
        native-lib

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        fmod
        fmodL #省略lib。具体的库链接到libnative-lib.so
        )