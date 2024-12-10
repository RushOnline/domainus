cmake_minimum_required(VERSION 3.16)

project(test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Отключаем использование getrandom API в Boost.Filesystem
set(BOOST_FILESYSTEM_DISABLE_GETRANDOM ON CACHE BOOL "Disable usage of getrandom API in Boost.Filesystem")

# Собираем только необходимые библиотеки Boost
set(BOOST_INCLUDE_LIBRARIES filesystem property_tree variant asio lexical_cast algorithm)

include(FetchContent)

FetchContent_Declare(
    curl
	URL https://curl.se/download/curl-7.76.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP true
    OVERRIDE_FIND_PACKAGE
)

FetchContent_Declare(
  boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
  DOWNLOAD_EXTRACT_TIMESTAMP true
  OVERRIDE_FIND_PACKAGE
)

# Эта переменная используется для сборки TgBot
foreach(_lib IN LISTS BOOST_INCLUDE_LIBRARIES)
    list(APPEND Boost_LIBRARIES boost_${_lib})    
endforeach()
unset(_lib)

# Эта переменная тоже используется для сборки TgBot
set(CURL_LIBRARIES libcurl)

# Поддержка c++17 filesystem в роутере отсутствует!
set(TGBOT_DISABLE_FILESYSTEM ON)


FetchContent_Declare(
    tgbot
    URL https://github.com/reo7sp/tgbot-cpp/archive/refs/tags/v1.8.zip
    PATCH_COMMAND patch -p1 < ${CMAKE_CURRENT_SOURCE_DIR}/tgbot-v1.8.patch
    DOWNLOAD_EXTRACT_TIMESTAMP true
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(curl boost tgbot)

add_executable(domainus main.cpp)
target_link_libraries(domainus PRIVATE TgBot)
