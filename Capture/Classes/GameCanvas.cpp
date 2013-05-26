#include "GameCanvas.h"

GameCanvas::GameCanvas() {
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	m_rt = CCRenderTexture::create(winSize.width, winSize.height);
	m_rt->setPosition(winSize.width/2,winSize.height/2);
	m_rt->retain();
}

GameCanvas::~GameCanvas() {
	m_rt->release();
}

void GameCanvas::drawPoint(CCPoint p, ccColor4F color) {
	m_rt->begin();
	ccDrawColor4F(color.r, color.g, color.b, color.a);
	ccDrawPoint(p);
	m_rt->end();
}

void GameCanvas::drawLine(CCPoint p1, CCPoint p2, ccColor4F color) {
	m_rt->begin();
	ccDrawColor4F(color.r, color.g, color.b, color.a);
	ccDrawLine(p1,p2);
	m_rt->end();
}

void GameCanvas::drawRect(CCPoint p1, CCPoint p2, ccColor4F color) {
	m_rt->begin();
	ccDrawColor4F(color.r, color.g, color.b, color.a);
	ccDrawRect(p1,p2);
	m_rt->end();
}

void GameCanvas::drawSolidRect(CCPoint p1, CCPoint p2, ccColor4F bColor, ccColor4F fColor) {

}

void GameCanvas::drawCircle(CCPoint p, int radius, ccColor4F color) {
	m_rt->begin();
	ccDrawColor4F(color.r, color.g, color.b, color.a);
	ccDrawCircle(p,radius,0,50,false);
	m_rt->end();
}

void GameCanvas::drawSolidCircle(CCPoint p, int radius, ccColor4F bColor, ccColor4F fColor) {

}

void GameCanvas::clear() {
	m_rt->clear(0,0,0,0);
}