#pragma once
#include <vector>
#include <Bullet/btBulletDynamicsCommon.h>
#include "ConsoleManager.h"
#include "RigidBody.h"

class RigidBody;

/// <summary>
/// Physics Engine Manager
/// </summary>
class PhysicsManager {
private:
    btDiscreteDynamicsWorld* world; ///< physics world
    btBroadphaseInterface* broadphase; ///< broad phase collision detection
    btDefaultCollisionConfiguration* collisionConfig; ///< collision configuration
    btCollisionDispatcher* dispatcher; ///< managing collisions between objects
    btSequentialImpulseConstraintSolver* solver; ///< Manages the resolution of physical constraints
    std::vector<RigidBody*> rigidBodies; ///< list of rigid bodies managed by the physics engine
    btRigidBody* ground;
    float gravity = -9.81f;

public:
    PhysicsManager();

    /// <summary>
    /// Initialize the Bullet world
    /// </summary>
    void initPhysics();

    /// <summary>
    /// Update the simulation
    /// </summary>
    /// <param name="deltaTime">float</param>
    void updatePhysics(float deltaTime);

    /// <summary>
    /// Destroy the physics engine
    /// </summary>
    void destroyPhysics();

    /// <summary>
    /// Add a rigid body to the physics engine
    /// </summary>
    /// <param name="body">RigidBody : the rigid body to add</param>
    void addRigidBody(RigidBody* body);

    /// <summary>
    /// Remove a rigid body to the physics engine
    /// </summary>
    /// <param name="body">RigidBody : the rigid body to destroy</param>
    void removeRigidBody(RigidBody* body);

    void addGroundBody(RigidGround* body);

    void removeGroundBody(RigidGround* body);

    /// <summary>
    /// getter to retrieve the world
    /// </summary>
    /// <returns>Pointer to btDiscreteDynamicsWorld</returns>
    btDiscreteDynamicsWorld* getWorld() { return world; }

    std::vector<RigidBody*> getRigidBodies() { return rigidBodies; }

    float getGravity() { return gravity; }

    btRigidBody* getGroundBody() { return ground; }
};
