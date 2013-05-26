#ifndef LOADINGSCENE_H
#define LOADINGSCENE_H


#include "cocos2d.h"
USING_NS_CC;


//------------------------------------------loadingLayer
class LoadingLayer:public CCLayer
{
public:
	virtual bool init();
	void loadingCallBack(CCObject* obj);

public:
	//ʹ�ú궨��create����
	CREATE_FUNC(LoadingLayer);
};

//------------------------------------------loadingScene
class LoadingScene:public CCScene
{
public:
	virtual bool init() {
		LoadingLayer *loadingLayer = LoadingLayer::create();
		this->addChild(loadingLayer);
		return true;
	}

public:
	//ʹ�ú궨��create����
	CREATE_FUNC(LoadingScene);
};
#endif
