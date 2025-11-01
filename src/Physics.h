#pragma once
#include "Module.h"
#include "Entity.h"
#include <list>
#include <cmath>
#include <box2d/box2d.h>

#define GRAVITY_X 0.0f
#define GRAVITY_Y -10.0f

#define PIXELS_PER_METER 50.0f
#define METER_PER_PIXEL  0.02f

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * (m)))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * (p))

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f

enum bodyType {
    DYNAMIC,
    STATIC,
    KINEMATIC
};

enum class ColliderType {
    PLAYER,
    ITEM,
    PLATFORM,
    ONE_WAY_PLATFORM, // <-- A�ADIDO
    ENEMY,
    WATER,
    UNKNOWN
};

class PhysBody
{
public:
    PhysBody() : listener(NULL), body(b2_nullBodyId), ctype(ColliderType::UNKNOWN) {}
    ~PhysBody() {}

    void  GetPosition(int& x, int& y) const;
    float GetRotation() const;
    bool  Contains(int x, int y) const;
    int   RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
    b2BodyId body;
    Entity* listener;
    ColliderType ctype;
};

class Physics : public Module
{
public:
    Physics();
    ~Physics();

    bool Start();
    bool PreUpdate();
    bool PostUpdate();
    bool CleanUp();

    PhysBody* CreateRectangle(int x, int y, int width, int height, bodyType type);
    PhysBody* CreateCircle(int x, int y, int radious, bodyType type);
    PhysBody* CreateRectangleSensor(int x, int y, int width, int height, bodyType type);
    PhysBody* CreateChain(int x, int y, int* points, int size, bodyType type);

    void BeginContact(b2ShapeId shapeA, b2ShapeId shapeB);
    void EndContact(b2ShapeId shapeA, b2ShapeId shapeB);

    void DeletePhysBody(PhysBody* physBody);
    bool IsPendingToDelete(PhysBody* physBody);

    b2Vec2 GetLinearVelocity(const PhysBody* p) const;
    float  GetXVelocity(const PhysBody* p) const;
    float  GetYVelocity(const PhysBody* p) const;

    void   SetLinearVelocity(PhysBody* p, const b2Vec2& v) const;
    void   SetLinearVelocity(PhysBody* p, float vx, float vy) const;
    void   SetXVelocity(PhysBody* p, float vx) const;
    void   SetYVelocity(PhysBody* p, float vy) const;

    void   ApplyLinearImpulseToCenter(PhysBody* p, float ix, float iy, bool wake = true) const;

    void SetPosition(PhysBody* p, float x, float y) const;

private:
    static b2BodyType ToB2Type(bodyType t);
    static void* ToUserData(PhysBody* p) { return (void*)p; }
    static PhysBody* FromUserData(void* ud) { return (PhysBody*)ud; }
    static PhysBody* BodyToPhys(b2BodyId b) { return FromUserData(b2Body_GetUserData(b)); }

    static void DrawSegmentCb(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* ctx);
    static void DrawPolygonCb(const b2Vec2* verts, int count, b2HexColor color, void* ctx);
    static void DrawSolidPolygonCb(b2Transform xf, const b2Vec2* verts, int count, float radius, b2HexColor color, void* ctx);
    static void DrawCircleCb(b2Vec2 center, float radius, b2HexColor color, void* ctx);
    static void DrawSolidCircleCb(b2Transform xf, float radius, b2HexColor color, void* ctx);

    static void DrawSolidCapsuleStub(b2Vec2 a, b2Vec2 b, float r, b2HexColor c, void* ctx);
    static void DrawPointStub(b2Vec2 p, float size, b2HexColor c, void* ctx);
    static void DrawStringStub(b2Vec2 p, const char* s, b2HexColor c, void* ctx);
    static void DrawTransformStub(b2Transform xf, void* ctx);

private:
    bool debug;
    b2WorldId world;
    std::list<PhysBody*> bodiesToDelete;
};