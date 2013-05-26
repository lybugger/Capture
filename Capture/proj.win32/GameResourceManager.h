#ifndef GAMERESOURCEMANAGER_H
#define GAMERESOURCEMANAGER_H


#include "cocos2d.h"
#include "LoadingScene.h"
USING_NS_CC;

class GameResourceManager
{
private:
	GameResourceManager(void);
	~GameResourceManager(void);

public:
	static GameResourceManager *sharedResource();
	bool loadResource(std::string);
	bool loadResource(LoadingLayer *load);
	bool layerInit(std::string, CCLayer *layer);
private:
	static GameResourceManager *m_resourceManager;
};

#endif
