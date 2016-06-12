//
//  Log.hpp
//  VideoPlayer
//
//  Created by Yunchao Chen on 16/6/12.
//
//

#ifndef _VIDEO_PLAYER_LOG_H_
#define _VIDEO_PLAYER_LOG_H_

//#define ANDROID_PLATFORM

#ifdef ANDROID_PLATFORM
#include "android/log.h"
#else
#include <stdio.h>
#endif

#ifdef ANDROID_PLATFORM
#define vLOGE(...) __android_log_print(ANDROID_LOG_ERROR, "VideoPlayer", __VA_ARGS__)
#define LOG_HERE() __android_log_print(ANDROID_LOG_ERROR, "VideoPlayer", "%s:+%d", __FILE__, __LINE__)
#define LOG_START() __android_log_print(ANDROID_LOG_ERROR, "VideoPlayer", "Enter: %s", __FUNCTION__)
#define LOG_END() __android_log_print(ANDROID_LOG_ERROR, "VideoPlayer", "Exit: %s", __FUNCTION__)
#else
#define vLOGE(...) \
    do {\
        printf("VideoPlayer: "); \
        printf(__VA_ARGS__); \
        putchar('\n'); \
    }while(0)
#define LOG_HERE() printf("VideoPlayer %s:+%d\n", __FILE__, __LINE__)
#define LOG_START() printf("VideoPlayer Enter:%s\n", __FUNCTION__)
#define LOG_END() printf("VideoPlayer Exit:%s\n", __FUNCTION__)
#endif



#endif //_VIDEO_PLAYER_LOG_H_
