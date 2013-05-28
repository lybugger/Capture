#ifndef ActorMANAGER_H
#define ActorMANAGER_H

#include "Box2D\Box2D.h"
#include "Actor.h"
#include "GameManager.h"
#include <list>


//设计为单例
class ActorManager : public GameManager{
public:
	static ActorManager *shareActorManager();
	void release();

private:
	static ActorManager *m_ActorManager;
	ActorManager();
	~ActorManager();

public:
	void updateWorld(float dt);
	virtual void BeginContact(b2Contact* contact);
	Actor *createActor(Actor::ActorType actorType, CCLayer *layer, CCPoint pos, float radius);
	bool clearActor(Actor *actor);
	void createWrapWall(CCLayer *layer, const CCPoint &pos, const CCSize &size);
	list<Actor *> *getActors() {return &m_actors;}
	ControlActor *getControlActor() {return m_controlActor;}
	void setControlActor(ControlActor *actor) {m_controlActor=actor;}
	void launch(CCPoint pos);
	bool isOver() {return m_over;}

protected:
	ControlActor *m_controlActor;
	WallActor *m_wall;
	list<Actor *> m_actors;
	list<Actor *> m_destroy;
	bool m_over;
};

#endif
