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

		if(a1->getType() == Actor::AT_Sun) a1->dealContact(a2);
		else if(a2->getType() == Actor::AT_Sun) a2->dealContact(a1);
		else if(a1->getType() == Actor::AT_Move || a2->getType() == Actor::AT_Move) a1->dealContact(a2);
	}
 
	B2_NOT_USED(contact); 
}

void ActorManager::updateWorld(float dt) {
	//------------------更新物理世界
	m_world->Step(dt, 8, 1);

	
	for(b2Body *actor_body = m_world->GetBodyList();actor_body;actor_body=actor_body->GetNext()) {
		Actor *actor = (Actor *)actor_body->GetUserData();
		if(actor==NULL) break;
		actor->update(dt);
	}

	//------------------删除吞噬的对象
	for(list<Actor *>::iterator itr = m_destroy.begin();itr!=m_destroy.end();) {
		delete *itr;
		itr = m_destroy.erase(itr);
	}
}

Actor *ActorManager::createActor(Actor::ActorType actorType, CCLayer *layer, CCPoint pos, float radius) {
	Actor *actor;
	if(actorType == Actor::AT_Control) {
		actor = new ControlActor(m_world, layer, pos, radius);
		m_controlActor = static_cast<ControlActor *>(actor);
		m_actors.push_back(actor);
	}else if(actorType == Actor::AT_Move) {
		actor = new MoveActor(m_world, layer, pos, radius);
		m_actors.push_back(actor);
	}else if(actorType == Actor::AT_Sun) {
		actor = new SunActor(m_world, layer, pos, radius,10000);
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
		m_controlActor = NULL;
		m_actors.remove(actor);
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
	Actor *actor = m_controlActor->launch(pos);
	if(actor) {
		m_actors.push_back(actor);
	}
}