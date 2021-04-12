# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.10)

set(SAMPLE_NAME "crawler")

set(SAMPLE_SRCS "crawl.cpp")

# dependent libraries
find_package(Poco REQUIRED Foundation Data DataSQLite Net NetSSL Crypto CONFIG)
find_package(easyhttpcpp REQUIRED CONFIG)

add_executable(${SAMPLE_NAME} ${SAMPLE_SRCS})
target_link_libraries(${SAMPLE_NAME}
                      easyhttpcpp::easyhttp
                      Poco::Foundation
                      Poco::Data Poco::DataSQLite
                      Poco::Net Poco::NetSSL Poco::Crypto
                      ssl crypto
                      )