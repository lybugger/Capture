#include "GameManager.h"
#include "GLES-Render.h"


GameManager::GameManager() 
	:m_mouseJoint(NULL),
	m_debugDraw(NULL)
{
	m_world = createWorld();
	b2BodyDef bodyDef;
    m_ground = m_world->CreateBody(&bodyDef);
}

GameManager::GameManager(b2World *world)
	:m_world(world),
	m_mouseJoint(NULL),
	m_debugDraw(NULL)
{
	b2BodyDef bodyDef;
    m_ground = m_world->CreateBody(&bodyDef);
}

/**
*析构函数
*/
GameManager::~GameManager(){
	LYDELETE(m_world)
	LYDELETE(m_debugDraw)
	LYDELETE(m_mouseJoint)
}

/**
*更新世界
*/
void GameManager::updateWorld(float dt) {
	m_world->Step(dt, 8, 1);
	//m_world->ClearForces();
	//m_world->DrawDebugData();

	for(b2Body *body = m_world->GetBodyList();body;body=body->GetNext()) {
		if(body->GetUserData() != NULL) {
			CCSprite* actor = (CCSprite *)body->GetUserData();
			//更新位置
			actor->setPosition(ccp(body->GetPosition().x*PTM_RATIO,body->GetPosition().y*PTM_RATIO));
			//更新角度
			actor->setRotation(-1*CC_RADIANS_TO_DEGREES(body->GetAngle()));
		}
	}
}

/**
*创建调试对象显示
*/
void GameManager::createDebug() {
	m_debugDraw = new GLESDebugDraw(PTM_RATIO);
	m_debugDraw->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit);
	m_world->SetDebugDraw(m_debugDraw);
}


/**
*创建方形
*/
b2Body *GameManager::createBox(const CCPoint &pos, const CCSize &size, bool isStatic,CCSprite *userData){
	b2BodyDef bodyDef;
	bodyDef.type = isStatic?b2_staticBody : b2_dynamicBody;
	bodyDef.position.Set(pos.x/PTM_RATIO,pos.y/PTM_RATIO);
	//bodyDef.bullet = true;
	
	//为box披上外衣
	if(userData != NULL) {
		bodyDef.userData = (CCSprite *)userData;
		userData->setTextureRect(CCRect(0,0,size.width,size.height));
	}

	//创建box
	b2PolygonShape boxShape;
	boxShape.SetAsBox(size.width/PTM_RATIO/2, size.height/PTM_RATIO/2);
	b2Body *box = m_world->CreateBody(&bodyDef);
	//设置夹具
	b2FixtureDef fixtureDef;
	fixtureDef.density = 10;			//密度
	fixtureDef.friction = 0.0f;			//滑动摩擦
	fixtureDef.restitution = 1.0f;		//弹性体复原
	fixtureDef.shape = &boxShape;
	box->CreateFixture(&fixtureDef);
	
	return box;
}

/**
*创建圆形
*/
b2Body *GameManager::createCircle(const CCPoint &pos, float radius, bool isStatic,CCSprite *userData){
	b2BodyDef bodyDef;
	bodyDef.type = isStatic?b2_staticBody : b2_dynamicBody;
	bodyDef.position.Set(pos.x/PTM_RATIO,pos.y/PTM_RATIO);
	
	//为box披上外衣
	if(userData != NULL) {
		bodyDef.userData = (CCSprite *)userData;
		userData->setTextureRect(CCRect(0,0, radius*2, radius*2));
	}

	//创建box
	b2CircleShape circleShape;
	circleShape.m_radius = radius/PTM_RATIO;
	b2Body *circle = m_world->CreateBody(&bodyDef);
	//设置夹具
	b2FixtureDef fixtureDef;
	fixtureDef.density = 3;			//密度
	fixtureDef.friction = 0.3f;			//滑动摩擦
	fixtureDef.restitution = 0.3f;		//弹性体复原
	fixtureDef.shape = &circleShape;
	circle->CreateFixture(&fixtureDef);
	
	return circle;
}


/**
*创建多边形
*/
b2Body *GameManager::createPolygon(const std::vector<CCPoint> &vertices,bool isStatic, CCSprite *userData){
	b2BodyDef bodyDef;
	bodyDef.type = isStatic?b2_staticBody : b2_dynamicBody;
	bodyDef.position.Set(0,0);
	
	//设置夹具
	b2FixtureDef fixtureDef;
	fixtureDef.density = 3;			//密度
	fixtureDef.friction = 0.3f;			//滑动摩擦
	fixtureDef.restitution = 0.3f;		//弹性体复原
	
	//创建polygon
	b2EdgeShape edgeShape;
	edgeShape;

	return NULL;
}


/**
*创建围栏
*/
void GameManager::createWrapWall(const CCPoint &pos, const CCSize &size){
	int thick = 20;

	createBox(CCPoint(pos.x + size.width/2, pos.y + size.height),CCSize(size.width,thick),true);
	createBox(CCPoint(pos.x + size.width/2, pos.y),CCSize(size.width,thick),true);
	createBox(CCPoint(pos.x, pos.y + size.height/2),CCSize(thick,size.height),true);
	createBox(CCPoint(pos.x + size.width, pos.y + size.height/2),CCSize(thick,size.height),true);
}

class QueryCallback : public b2QueryCallback
{
public:
    QueryCallback() 
		:m_fixture(NULL)
	{
	
	}

    /// Called for each fixture found in the query AABB.
    /// @return false to terminate the query.
	virtual bool ReportFixture(b2Fixture* fixture)  {
		 b2Body* body = fixture->GetBody();
         m_fixture = fixture;
        return true;
	}

    b2Fixture* m_fixture;
};

/**
*获取某位置下的刚体
*/
b2Body *GameManager::getBodyByPosition(const CCPoint &pos){

	b2Vec2 bPoint = b2Vec2(pos.x/PTM_RATIO, pos.y/PTM_RATIO);
	b2Body *body = NULL;
	b2AABB aabb;

	//设置该点范围
	aabb.lowerBound.Set(bPoint.x-0.001,bPoint.y-0.001);
	aabb.upperBound.Set(bPoint.x+0.001,bPoint.y+0.001);


	QueryCallback callback;
	m_world->QueryAABB(&callback,aabb);
	if(callback.m_fixture) body = callback.m_fixture->GetBody();
	

	return body;
}


/**
*拖动刚体
*/
bool GameManager::startDragBody(const CCPoint &pos){
	b2Body *body = getBodyByPosition(pos);

	if(body == NULL) return false;

	b2MouseJointDef mouseJointDef;
	mouseJointDef.bodyA = m_ground;
	mouseJointDef.bodyB = body;
	mouseJointDef.target = b2Vec2(pos.x/PTM_RATIO, pos.y/PTM_RATIO);
	mouseJointDef.maxForce = 1000.0f*body->GetMass();
	m_mouseJoint = (b2MouseJoint*)m_world->CreateJoint(&mouseJointDef);
    body->SetAwake(true);
	return true;
}

bool GameManager::underDragBody(const CCPoint &pos) {
	if(m_mouseJoint != NULL) {
		m_mouseJoint->SetTarget(b2Vec2(pos.x/PTM_RATIO,pos.y/PTM_RATIO));
		return true;
	}
	return false;
}

/**
*停止拖动刚体
*/
bool GameManager::stopDragBody(){
	if (m_mouseJoint)
    {
        m_world->DestroyJoint(m_mouseJoint);
        m_mouseJoint = NULL;
		return true;
    }
	return false;
}



