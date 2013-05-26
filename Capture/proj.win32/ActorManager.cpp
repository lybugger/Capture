#include "ActorManager.h"

ActorManager *ActorManager::m_ActorManager = new ActorManager();
ActorManager *ActorManager::shareActorManager() {
	if(!m_ActorManager) {
		m_ActorManager = new ActorManager;
	}
	return m_ActorManager;
}
void ActorManager::release() {
	CC_SAFE_DELETE(m_ActorManager);
}

//构造函数
ActorManager::ActorManager() 
	:GameManager()
{
	m_world->SetGravity(b2Vec2(0,0));
	m_world->SetContactListener(this);
}

//析构函数
ActorManager::~ActorManager() {
	release();

	//clear move
	for(list<Actor *>::iterator itr = m_actors.begin();itr!=m_actors.end();) {
		itr = m_actors.erase(itr);
	}
	//clear wall
	for(list<Actor *>::iterator itr = m_wall.begin();itr!=m_wall.end();) {
		itr = m_wall.erase(itr);
	}

}

void ActorManager::BeginContact(b2Contact* contact) 
{ 
	if(contact) {
		b2Body *b1 = contact->GetFixtureA()->GetBody();
		b2Body *b2 = contact->GetFixtureB()->GetBody();
		if(((Actor *)b1->GetUserData())->getType() != Actor::AT_Wall 
			&& ((Actor *)b2->GetUserData())->getType() != Actor::AT_Wall) 
		{

			Actor *ba1 = (Actor *)b1->GetUserData();
			Actor *ba2 = (Actor *)b2->GetUserData();
			MoveActor *a1 = static_cast<MoveActor *>(ba1);
			MoveActor *a2 = static_cast<MoveActor *>(ba2);

			if(a1->getMass() > a2->getMass()) {
				a1->increase(a2->getMass());
				a2->reduce(a2->getMass());
				m_actors.remove(a2);
					delete a2;
			}else {
				a2->increase(a1->getMass());
				a1->reduce(a1->getMass());
				m_actors.remove(a1);
				delete a1;
			}
			
		}
	}
 
	B2_NOT_USED(contact); 
}

void ActorManager::updateWorld(float dt) {
	m_world->Step(dt, 8, 1);

	for(b2Body *body = m_world->GetBodyList();body;body=body->GetNext()) {
		if(body->GetUserData() != NULL) {
			CCSprite* actor = ((Actor *)body->GetUserData())->getShape();
			//更新位置
			actor->setPosition(ccp(body->GetPosition().x*PTM_RATIO,body->GetPosition().y*PTM_RATIO));
			//更新角度
			actor->setRotation(-1*CC_RADIANS_TO_DEGREES(body->GetAngle()));

			//if(body->GetMass() > m_cActor)

		}
	}

}

Actor *ActorManager::createActor(Actor::ActorType actorType, CCLayer *layer, CCPoint pos, float radius) {
	Actor *actor;
	if(actorType == Actor::AT_Control) {
		actor = new ControlActor(m_world, layer, pos, radius);
		m_cActor = (ControlActor *)actor;
	}else if(actorType == Actor::AT_Move) {
		actor = new MoveActor(m_world, layer, pos, radius);
		m_actors.push_back(actor);
	}

	return actor;
}


/**
*创建围栏
*/
void ActorManager::createWrapWall(CCLayer *layer, const CCPoint &pos, const CCSize &size){
	int thick = 20;
	
	m_wall.push_back(
		new WallActor(m_world, layer, CCPoint(pos.x + size.width/2, pos.y + size.height),CCSize(size.width,thick)));
	m_wall.push_back(
		new WallActor(m_world, layer, CCPoint(pos.x + size.width/2, pos.y),CCSize(size.width,thick)));
	m_wall.push_back(
		new WallActor(m_world, layer, CCPoint(pos.x, pos.y + size.height/2),CCSize(thick,size.height)));
	m_wall.push_back(
		new WallActor(m_world, layer, CCPoint(pos.x + size.width, pos.y + size.height/2),CCSize(thick,size.height)));
}

void ActorManager::launch(CCPoint pos) {
	BaseActor *actor = m_cActor->launch(pos);
	if(actor) {
		m_actors.push_back(actor);
	}
}