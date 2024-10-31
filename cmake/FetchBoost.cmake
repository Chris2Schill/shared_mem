set(BOOST_ENABLE_CMAKE ON)
include(FetchContent)
FetchContent_Declare(
  boost_interprocess
  GIT_REPOSITORY https://github.com/boostorg/interprocess.git
  GIT_TAG boost-1.86.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(boost_interprocess)
