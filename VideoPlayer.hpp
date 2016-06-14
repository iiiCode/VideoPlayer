//
//  VideoPlayer.hpp
//  VideoPlayer
//
//  Created by Yunchao Chen on 16/6/7.
//
//

#ifndef _VIDEO_PLAYER_H_
#define _VIDEO_PLAYER_H_

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif

#include <pthread.h>

#include "cocos2d.h"
#include "RingBuffer.hpp"

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}

USING_NS_CC;

class VideoPlayer : public Sprite {
public:
    VideoPlayer();
    virtual ~VideoPlayer();
    
    static VideoPlayer *create(const char *file, int width, int height);
    
public:
    void start();
    void stop();
    void pause(bool _pause);
    void seek(int64_t seekTime);
    void accurateSeek(int64_t seekTime);
    void setTimeScale(float scale);
    void setPlaybackEndCallback(void (callback)(VideoPlayer *, const char *));
    virtual void update(float delta);
    void draw(Renderer *renderer, const Mat4& transform, uint32_t flags);

protected:
    bool init(const char *path, int width, int height);
    
private:
    void doSeek();
    void doAccurateSeek();
    void dump();
    void decodeToSeekTime();
    static void *doTimeCounter(void *args);
    static void doUpdatePicture(void *args);
    static void *doProcessVideo(void *args);
    static void pictureDestroy(DataType *item);
private:
    bool mStop;
    bool mPause;
    bool mSeek;
    bool mAccurateSeek;
    AVFormatContext *mFormatCtx;
    AVCodecContext *mCodecCtx;
    AVFrame* mFrame;
    int mVideoStreamIndex;
    SwsContext *mImageConvertCtx;
    
    int mWidth;
    int mHeight;
    
    float mTimeScale;
    int64_t mSeekTime;
    
    void (*mVideoEndCallback)(VideoPlayer *, const char *);
    RingBuffer mPictureRingBuffer;
  
    std::string mFilePath;
    
    char *mPath;
    
    AVPicture *mPicture;
    int64_t mTimeBase;
    int mFPS;
    
    pthread_t mDcoderThread;
    pthread_t mRenderThread;
    pthread_mutex_t mRenderMutex;
};

#endif //_VIDEO_PLAYER_H_

