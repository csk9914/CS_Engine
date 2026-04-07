cmake_minimum_required(VERSION 3.20)
project(CS_Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── 소스 파일 목록 ─────────────────────────────────────────────────────
# 2. ImGui 관련 파일만 명시적으로 따로 정의
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/Src/imgui")
file(GLOB IMGUI_SOURCES 
    "${IMGUI_DIR}/*.cpp"
    "${IMGUI_DIR}/*.h"
    "${IMGUI_DIR}/backends/imgui_impl_win32.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_win32.h"
    "${IMGUI_DIR}/backends/imgui_impl_dx11.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_dx11.h"
)

file(GLOB_RECURSE SOURCES "Src/*.cpp")
file(GLOB_RECURSE HEADERS "Src/*.h")
file(GLOB_RECURSE SHADERS "Src/Shaders/*.hlsl")

# 중복 제거 (SOURCES와 IMGUI_SOURCES에 겹치는 파일이 있을 경우 제거)
if(IMGUI_SOURCES)
    list(REMOVE_ITEM SOURCES ${IMGUI_SOURCES})
    list(REMOVE_ITEM HEADERS ${IMGUI_SOURCES})
endif()

# 각 셰이더 파일의 속성을 '빌드 제외'로 설정
set_source_files_properties(${SHADERS} PROPERTIES HEADER_FILE_ONLY TRUE)

# ── 실행 파일 (Windows 앱 - 콘솔 없음) ──────────────────────────────
add_executable(CS_Engine WIN32 ${SOURCES} ${HEADERS} ${IMGUI_SOURCES} ${SHADERS})

# ── 인클루드 경로 ──────────────────────────────────────────────────────
target_include_directories(CS_Engine PRIVATE 
    "Src"
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)

# ── Windows / DirectX 라이브러리 링크 ────────────────────────────────
target_link_libraries(CS_Engine PRIVATE
    d3d11.lib
    d3dcompiler.lib
    dxgi.lib
    dxguid.lib
)

# 빌드가 끝난 후 Shaders 폴더를 실행 파일 위치로 복사
add_custom_command(TARGET CS_Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Src/Shaders"
        "$<TARGET_FILE_DIR:CS_Engine>/Shaders"
    COMMENT "Copying shaders to build directory..."
)

# ── VS 필터(폴더) 설정 ────────────────────────────────────────────────
source_group("Source Files" FILES ${SOURCES})
source_group("Header Files" FILES ${HEADERS})
source_group("Shaders" FILES ${SHADERS})
source_group("ImGui" FILES ${IMGUI_SOURCES})

# 유니코드 전처리기 정의 추가
add_compile_definitions(UNICODE _UNICODE)

# 실행 파일 생성 후 아래 줄 추가
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT CS_Engine)

# 실행 파일(CS_Engine) 생성 코드 아래에 추가하세요.
set_target_properties(CS_Engine PROPERTIES
    # 디버깅 시 작업 디렉터리를 소스 폴더(Src)나 프로젝트 루트로 설정
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Src"
)