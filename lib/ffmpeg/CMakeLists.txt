cmake_minimum_required(VERSION 3.25.1)
project(ffmpeg)

find_package(PkgConfig REQUIRED)
pkg_check_modules(AVCODEC      REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT     REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVFILTER     REQUIRED IMPORTED_TARGET libavfilter)
pkg_check_modules(AVDEVICE     REQUIRED IMPORTED_TARGET libavdevice)
pkg_check_modules(AVUTIL       REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(SWRESAMPLE   REQUIRED IMPORTED_TARGET libswresample)
pkg_check_modules(SWSCALE      REQUIRED IMPORTED_TARGET libswscale)

add_library(ffmpeg INTERFACE IMPORTED GLOBAL)
target_include_directories(ffmpeg INTERFACE
	${AVCODEC_INCLUDE_DIRS}
	${AVFORMAT_INCLUDE_DIRS}
	${AVFILTER_INCLUDE_DIRS}
	${AVDEVICE_INCLUDE_DIRS}
	${AVUTIL_INCLUDE_DIRS}
	${SWRSAMPLE_INCLUDE_DIRS}
	${SWSCALE_INCLUDE_DIRS}
)
target_link_options(ffmpeg INTERFACE
	${AVCODEC_LDLFLAGS}
	${AVFORMAT_LDLFLAGS}
	${AVFILTER_LDLFLAGS}
	${AVDEVICE_LDLFLAGS}
	${AVUTIL_LDLFLAGS}
	${SWRSAMPLE_LDLFLAGS}
	${SWSCALE_LDLFLAGS}
)
