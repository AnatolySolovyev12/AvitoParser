# Указываем минимальную версию CMake
cmake_minimum_required(VERSION 3.10)

#Даём название собираемому проекту
project(TestProject)


#Задаём стандарт языка для собираемого проекта
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Network Svg Core)


# Исходник приложения

file(GLOB APP_SOURCE "${CMAKE_SOURCE_DIR}/AvitoParser/*.cpp")

set(APP_SOURCE "${CMAKE_SOURCE_DIR}/AvitoParser/AvitoParser.ui)

# Библиотека
qt_add_library(AvitoParserLIB STATIC ${APP_SOURCE})
target_include_directories(AvitoParserLIB PUBLIC ${CMAKE_SOURCE_DIR}/AvitoParser)


# Исполняемый файл
qt_add_executable(AvitoParserAPP ${APP_SOURCE})

#target_include_directories(AvitoParserAPP PUBLIC ${Qt6_INCLUDE_DIRS})

# Связываем приложение и библиотеку
target_link_libraries(AvitoParserAPP PUBLIC AvitoParserLIB Qt6::Widgets Qt6::Gui Qt6::Network Qt6::Svg Qt6::Core)
