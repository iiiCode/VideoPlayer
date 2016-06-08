#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "VideoPlayer.hpp"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    mPause = false;
    mVideoPlayer = VideoPlayer::create("/Users/ycchen/Video.mp4", 1024, 576);
    mVideoPlayer->setPlaybackEndCallback(videoEnd);
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...
    
    auto startItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuStartCallback, this));
    
    startItem->setPosition(Vec2(origin.x + visibleSize.width - startItem->getContentSize().width/2 ,
                                origin.y + startItem->getContentSize().height * 2));
    
    // create menu, it's an autorelease object
    auto startMenu = Menu::create(startItem, NULL);
    startMenu->setPosition(Vec2::ZERO);
    this->addChild(startMenu, 1);

    auto pauseItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuPauseCallback, this));
    
    pauseItem->setPosition(Vec2(origin.x + visibleSize.width - pauseItem->getContentSize().width/2 ,
                                origin.y + pauseItem->getContentSize().height * 3));
    
    // create menu, it's an autorelease object
    auto pauseMenu = Menu::create(pauseItem, NULL);
    pauseMenu->setPosition(Vec2::ZERO);
    this->addChild(pauseMenu, 1);
    
    auto stopItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuStopCallback, this));
    
    stopItem->setPosition(Vec2(origin.x + visibleSize.width - stopItem->getContentSize().width/2 ,
                                origin.y + stopItem->getContentSize().height * 4));
    
    // create menu, it's an autorelease object
    auto stopMenu = Menu::create(stopItem, NULL);
    stopMenu->setPosition(Vec2::ZERO);
    this->addChild(stopMenu, 1);
    
    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    
    this->addChild(mVideoPlayer);

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::menuStartCallback(Ref* pSender)
{
    mVideoPlayer->start();
}

void HelloWorld::menuPauseCallback(Ref* pSender)
{
    mPause = ! mPause;
    mVideoPlayer->pause(mPause);
}

void HelloWorld::menuStopCallback(Ref* pSender)
{
    mVideoPlayer->stop();
}

void HelloWorld::videoEnd(VideoPlayer *player, const char *info)
{
    if (strcmp(info, "stop") == 0) {
        player->stop();
    }
}