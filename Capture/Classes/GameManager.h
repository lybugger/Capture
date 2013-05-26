#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Box2D\Box2D.h"
#include "cocos2d.h"
USING_NS_CC;

#define EXAMINE_WORLD if(m_world == NULL) break;
#define PTM_RATIO 32
#define LYDELETE(name) if(name != NULL) {delete name; name = NULL;}

class GLESDebugDraw;
class GameManager : public b2ContactListener{

public:
	GameManager();
	GameManager(b2World *world);
	~GameManager();

	b2World *createWorld() {return new b2World(b2Vec2(0,-10));}
	b2World *getWorld() {return m_world;}
	void setWorld(b2World *world) {m_world = world;}
	void createDebug();
	b2Body *createBox(const CCPoint &pos, const CCSize &size, bool isStatic=false, CCSprite *userData = NULL);
	b2Body *createCircle(const CCPoint &pos, float radius, bool isStatic=false, CCSprite *userData = NULL);
	b2Body *createPolygon(const std::vector<CCPoint> &vertices, bool isStatic=false, CCSprite *userData = NULL);
	void createWrapWall(const CCPoint &pos, const CCSize &size);

	b2Body *getBodyByPosition(const CCPoint &pos);
	bool startDragBody(const CCPoint &pos);
	bool underDragBody(const CCPoint &pos);
	bool stopDragBody();
	void updateWorld(float dt);
	GLESDebugDraw *getDebugDraw() {return m_debugDraw;}

protected:
	b2World *m_world;
	b2Body *m_ground;
	GLESDebugDraw * m_debugDraw;
	b2MouseJoint *m_mouseJoint;
};

#endif