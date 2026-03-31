# 1. 최소 요구 버전 설정
cmake_minimum_required(VERSION 3.10)

# 2. 프로젝트 이름 및 언어 설정
project(CS_Engine_Project LANGUAGES CXX)

# 3. C++ 표준 설정 (C++17 또는 C++20 추천)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. 소스 파일들 등록 (본인의 파일 이름에 맞게 수정하세요)
# 모든 .cpp와 .h 파일을 읽어오도록 설정
# Only gather source files from the project Sources directory to avoid
# accidentally including files generated in the build directory
# (e.g. CMakeCXXCompilerId.cpp) which can cause duplicate symbol errors.
file(GLOB_RECURSE SOURCES "Src/*.cpp" "Src/*.h")

# 5. 실행 파일 생성 (프로젝트 이름, 소스 파일들)
add_executable(CS_Engine ${SOURCES})

# 6. 헤더 파일 경로 포함 (필요한 경우)
target_include_directories(CS_Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

