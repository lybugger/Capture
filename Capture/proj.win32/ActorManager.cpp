#include "ActorManager.h"
#include "GLES-Render.h"


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
	m_over = false;
}

//析构函数
ActorManager::~ActorManager() {
	release();
	//clear control actor
	if(m_cActor) {
		delete m_cActor;
		m_cActor = NULL;
	}

	//clear move actor
	for(list<Actor *>::iterator itr = m_actors.begin();itr!=m_actors.end();) {
		delete *itr;
		itr = m_actors.erase(itr);
	}

}


//碰撞开始
void ActorManager::BeginContact(b2Contact* contact) { 
	if(contact) {
		b2Body *b1 = contact->GetFixtureA()->GetBody();
		b2Body *b2 = contact->GetFixtureB()->GetBody();
		Actor *a1 = (Actor *)b1->GetUserData();
		Actor *a2 = (Actor *)b2->GetUserData();

		if(a1->getType() != Actor::AT_Wall && a2->getType() != Actor::AT_Wall) {

			MoveActor *ma1 = static_cast<MoveActor *>(a1);
			MoveActor *ma2 = static_cast<MoveActor *>(a2);

			//预防多个同时碰撞
			if(ma1->getDeltaMass() != 0 || ma2->getDeltaMass() !=0) return;

			if(ma1->getMass() > ma2->getMass()) {
				ma1->setDeltaMass(ma2->getMass());
				ma2->setDeltaMass(-ma2->getMass());
				this->clearActor(ma2);
			}else {
				ma2->setDeltaMass(ma1->getMass());
				ma1->setDeltaMass(-ma1->getMass());
				this->clearActor(ma1);
			}
			
		}
	}
 
	B2_NOT_USED(contact); 
}

void ActorManager::updateWorld(float dt) {
	//------------------删除吞噬的对象
	for(list<Actor *>::iterator itr = m_destroy.begin();itr!=m_destroy.end();) {
		delete *itr;
		itr = m_destroy.erase(itr);
	}



	//------------------更新物理世界
	m_world->Step(dt, 8, 1);

	
	for(b2Body *actor_body = m_world->GetBodyList();actor_body;actor_body=actor_body->GetNext()) {
		Actor *actor = (Actor *)actor_body->GetUserData();
		if(actor==NULL) break;

		//------------------更新 actor shape
		if(actor_body->GetUserData() != NULL) {
			CCSprite *actor_shape = actor->getShape();
			//更新位置
			actor_shape->setPosition(ccp(actor_body->GetPosition().x*PTM_RATIO,actor_body->GetPosition().y*PTM_RATIO));
			//更新角度
			actor_shape->setRotation(-1*CC_RADIANS_TO_DEGREES(actor_body->GetAngle()));
		}

		//-------------------更新质量
		if(actor->getType() == Actor::AT_Control || actor->getType() == Actor::AT_Move) {
			((MoveActor *)actor)->swallow();
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

bool ActorManager::clearActor(Actor *actor) {
	if(!actor) return false;

	if(actor->getType() == Actor::AT_Move) {
		m_actors.remove(actor);
		m_destroy.push_back(actor);
	}else if(actor->getType() == Actor::AT_Control){
		m_cActor = NULL;
		m_destroy.push_back(actor);
		m_over = true;
	}

	return true;
}

/**
*创建围栏
*/
void ActorManager::createWrapWall(CCLayer *layer, const CCPoint &pos, const CCSize &size){
	m_wall = new WallActor(m_world, layer, pos, size);
}

void ActorManager::launch(CCPoint pos) {
	Actor *actor = m_cActor->launch(pos);
	if(actor) {
		m_actors.push_back(actor);
	}
}