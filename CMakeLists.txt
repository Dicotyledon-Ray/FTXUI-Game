cmake_minimum_required(VERSION 3.14)
project(FTXUIGame VERSION 1.0.0 LANGUAGES CXX)

# 要求支持 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动下载和集成 FTXUI 库
include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG v5.0.0 # 可以根据需要选择稳定版本
)
FetchContent_MakeAvailable(ftxui)

# 包含头文件目录
include_directories(include)

# 添加可执行文件
add_executable(ftxui_game 
    src/main.cpp 
    src/Game.cpp # 记得把你的逻辑实现文件也写在这里
)

# 链接 FTXUI 库
target_link_libraries(ftxui_game PRIVATE ftxui::screen ftxui::dom ftxui::component)