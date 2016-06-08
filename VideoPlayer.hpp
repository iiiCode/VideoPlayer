//
//  VideoPlayer.hpp
//  VideoPlayer
//
//  Created by Yunchao Chen on 16/6/7.
//
//

#ifndef _VIDEO_PLAYER_H_
#define _VIDEO_PLAYER_H_

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
    void setTimeScale(int scale);
    void setPlaybackEndCallback(void (callback)());
    virtual void update(float delta);
    void draw(Renderer *renderer, const Mat4& transform, uint32_t flags);

protected:
    bool init(const char *path, int width, int height);
    
private:
    static void *doProcessVideo(void *args);
private:
    bool mStop;
    bool mPause;
    AVFormatContext *mFormatCtx;
    AVCodecContext *mCodecCtx;
    AVFrame* mFrame;
    int mVideoStreamIndex;
    SwsContext *mImageConvertCtx;
    
    int mWidth;
    int mHeight;
    
    int mDelta;
    
    void (*mVideoEndCallback)();
    RingBuffer mPictureRingBuffer;
  
    std::string mFilePath;
    
    pthread_t mPtid;
    
    char *mPath;
    Texture2D *mTexture;
};

#endif //_VIDEO_PLAYER_H_