//
//  VideoPlayer.cpp
//  VideoPlayer
//
//  Created by Yunchao Chen on 16/6/7.
//
//

#include <unistd.h>
#include "VideoPlayer.hpp"

#define LOG_START() printf("Enter: %s\n", __func__)
#define LOG_END()   printf("Exit: %s\n", __func__)

VideoPlayer::VideoPlayer()
: mStop(true),
mPause(false),
mFormatCtx(nullptr),
mCodecCtx(nullptr),
mFrame(nullptr),
mVideoStreamIndex(-1),
mImageConvertCtx(nullptr),
mWidth(0),
mHeight(0),
mVideoEndCallback(nullptr)
{
    LOG_START();
    LOG_END();
}

VideoPlayer::~VideoPlayer()
{
    LOG_START();
    free(mPath);
    LOG_END();
}

VideoPlayer *VideoPlayer::create(const char *file, int width, int height)
{
    VideoPlayer * player = new VideoPlayer();
    
    if (player && player->init(file, width, height)) {
        player->autorelease();
    } else {
        delete player;
        player = nullptr;
    }
    
    return player;
}

void VideoPlayer::stop()
{
    LOG_START();
    
    if (mStop) {
        return;
    }
    
    mStop = true;
    mPictureRingBuffer.notifyRingBufferExit();
    mPictureRingBuffer.flush();
    pthread_join(mPtid, nullptr);
    
    sws_freeContext(mImageConvertCtx);
    av_frame_free(&mFrame);
    avcodec_close(mCodecCtx);
    avformat_close_input(&mFormatCtx);
    
    LOG_END();
}

void VideoPlayer::pause(bool _pause)
{
    LOG_START();
    if (mStop) {
        return;
    }
    mPause = _pause;
    LOG_END();
}

void VideoPlayer::seek(int64_t seekTime)
{
    
}

void VideoPlayer::accurateSeek(int64_t seekTime)
{
    
}

void VideoPlayer::setTimeScale(int scale)
{
    
}

void VideoPlayer::setPlaybackEndCallback(void (*callback)(VideoPlayer *, const char *))
{
    mVideoEndCallback = callback;
}

bool VideoPlayer::init(const char *path, int width, int height)
{
    Sprite::init();
    CCDirector::sharedDirector()->getScheduler()->scheduleUpdateForTarget(this,0,false);
    
    mPath = strdup(path);
    mWidth = width;
    mHeight = height;
    
    return true;
}

void VideoPlayer::start()
{
    LOG_START();
    
    if (! mStop) {
        return;
    }
    
    mStop = false;
    av_register_all();
    
    mFilePath = CCFileUtils::sharedFileUtils()->fullPathForFilename(mPath);
    printf("file path: %s\n", mFilePath.c_str());
    
    if (avformat_open_input(&mFormatCtx, mFilePath.c_str(), nullptr, nullptr) != 0) {
        printf("avformat_open_input failed.\n");
        return;
    }
    
    if (avformat_find_stream_info(mFormatCtx, nullptr) < 0) {
        printf("avformat_find_stream_info failed.\n");
        return;
    }
    
    for (int i = 0; i < mFormatCtx->nb_streams; i ++) {
        if (mVideoStreamIndex == -1 && mFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            mVideoStreamIndex = i;
            break;
        }
    }
    
    printf("video stream index: %d\n", mVideoStreamIndex);
    
    if (mVideoStreamIndex == -1) {
        return;
    }
    
    if (mWidth <= 0) {
        mWidth = mFormatCtx->streams[mVideoStreamIndex]->codec->width;
    }
    
    if (mHeight <= 0) {
        mHeight = mFormatCtx->streams[mVideoStreamIndex]->codec->height;
    }
    
    printf("width: %d height: %d\n", mWidth, mHeight);
    mCodecCtx = mFormatCtx->streams[mVideoStreamIndex]->codec;
    
    AVCodec *codec = avcodec_find_decoder(mCodecCtx->codec_id);
    if (codec == nullptr) {
        printf("avcodec_find_decoder failed.\n");
        return;
    }
    
    if (avcodec_open2(mCodecCtx, codec, nullptr) != 0) {
        printf("avcodec_open2 failed.\n");
        return;
    }
    
    if ((mFrame = av_frame_alloc()) == nullptr) {
        printf("av_frame_alloc failed.\n");
    }
    
    mImageConvertCtx = sws_alloc_context();
    if (mImageConvertCtx == nullptr) {
        printf("sws_alloc_context failed.\n");
        return;
    }
    sws_init_context(mImageConvertCtx, nullptr, nullptr);    
    mImageConvertCtx = sws_getContext(mCodecCtx->width,
                                      mCodecCtx->height,
                                      mCodecCtx->pix_fmt,
                                      mWidth,
                                      mHeight,
                                      PIX_FMT_RGB24,
                                      SWS_FAST_BILINEAR, NULL, NULL, NULL);
    
    mTexture = new Texture2D();
    mTexture->autorelease();
    const CCSize& size = CCDirector::getInstance()->getWinSize();
    setPosition(Vec2(size.width, size.height));
    
    pthread_create(&mPtid, NULL, doProcessVideo, this);
    LOG_END();
}

//static
void *VideoPlayer::doProcessVideo(void *args)
{
    AVFrame *frame;
    AVPicture *picture;
    AVPacket packet;
    int got_frame = 0;
    
    VideoPlayer *player = static_cast<VideoPlayer*>(args);
    
    while(! player->mStop) {
        if (av_read_frame(player->mFormatCtx, &packet) < 0) {
            printf("END OF FILE.\n");
            player->mVideoEndCallback(player, "stop");
            av_free_packet(&packet);
            break;
        }
        
        if (packet.stream_index == player->mVideoStreamIndex) {
            
            avcodec_decode_video2(player->mCodecCtx, player->mFrame, &got_frame, &packet);
            
            if (got_frame == 0) {
                printf("Do not get a frame.\n");
                av_free_packet(&packet);
                continue;
            }
            
            picture = new AVPicture;
            avpicture_alloc(picture, PIX_FMT_RGB24, player->mWidth, player->mHeight);
            sws_scale (player->mImageConvertCtx,
                       player->mFrame->data,
                       player->mFrame->linesize,
                       0, player->mCodecCtx->height,
                       picture->data,
                       picture->linesize);
            

            player->mPictureRingBuffer.enqueue(picture);
            
        } else {
            printf("Not video stream packet, ignore it.\n");
        }
        
        av_free_packet(&packet);
        usleep(10);
        
    }
    
    return nullptr;
}

void VideoPlayer::update(float delta)
{
}

void VideoPlayer::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    AVPicture *picture;
    
    if (! mStop && ! mPause) {
        
        usleep(20 * 1000);
        mPictureRingBuffer.dequeue((DataType **)&picture);
 
        mTexture->initWithData(picture->data[0],
                               mWidth * mHeight,
                               kCCTexture2DPixelFormat_RGB888,
                               mWidth, mHeight, Size(mWidth, mHeight));
        initWithTexture(mTexture);
        setContentSize(Size(mWidth, mHeight));

        GL::bindTexture2D(mTexture->getName());
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     mWidth, mHeight, 0,
                     GL_RGB, GL_UNSIGNED_BYTE,
                     picture->data[0]);
        
        avpicture_free(picture);
        delete picture;
    }
    
    Sprite::draw(renderer, transform, flags);
}

