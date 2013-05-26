#include "GameResourceManager.h"

GameResourceManager *GameResourceManager::m_resourceManager = new GameResourceManager;

GameResourceManager::GameResourceManager(void)
{
	CCFileUtils::sharedFileUtils()->addSearchPath("D:/src");

}


GameResourceManager::~GameResourceManager(void)
{

}

GameResourceManager* GameResourceManager::sharedResource() {
	return m_resourceManager;
}


bool GameResourceManager::loadResource(std::string) {
	return true;
}

bool GameResourceManager::loadResource(LoadingLayer *load) {
	//test for test 131 jpg
	CCFileUtils::sharedFileUtils()->addSearchPath("D:/Funing_picture");

	char str[20];
	for(int i=0;i<10;i++) {
		sprintf(str,"test%d.jpg",i);
		CCTextureCache::sharedTextureCache()->addImageAsync(str, load, callfuncO_selector(LoadingLayer::loadingCallBack));
	}

	return true;
}

bool GameResourceManager::layerInit(std::string, CCLayer *layer) {
	return true;
}