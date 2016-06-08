#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "VideoPlayer.hpp"
#include "cocos2d.h"

class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    void menuStartCallback(cocos2d::Ref* pSender);
    void menuPauseCallback(cocos2d::Ref* pSender);
    void menuStopCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
private:
    static void videoEnd(VideoPlayer * player, const char *info);
private:
    bool mPause;
    VideoPlayer *mVideoPlayer;
};

#endif // __HELLOWORLD_SCENE_H__
