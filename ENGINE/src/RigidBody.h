#pragma once
#include <Bullet/btBulletDynamicsCommon.h>
#include "ObjectCollider.h"
#include "ConsoleManager.h"

/// <summary>
/// Class representing a rigid body in the physics engine
/// </summary>
class RigidBody {
private:
    btRigidBody* body; ///< Pointer to the Bullet rigid body (btRigidBody)
    Collider* collider; ///< Pointer to the Collider object
    btMotionState* motionState; ///< Pointer to the Bullet motion state (btMotionState)

public:
    RigidBody(Collider* collider, float mass, btVector3 position);

    /// <summary>
    /// Apply the force on the rigid body
    /// </summary>
    /// <param name="force">btVector3</param>
    void ApplyForce(const btVector3& force);

    /// <summary>
    /// Sets the velocity of the rigid body
    /// </summary>
    /// <param name="velocity">btVector3</param>
    void SetVelocity(const btVector3& velocity);

    void DoImpulse(const btVector3& impulse);

    void SetPosition(const btVector3& position);

    void SetAngularVelocity(const btVector3& angularVelocity);

    void ApplyTorque(const btVector3& torque);

    void SetRotation(const btQuaternion& rotation);




    btRigidBody* GetBulletBody() const { return body; }
};




class RigidGround
{
private:
    btRigidBody* groundBody; ///< Pointer to the Bullet rigid body (btRigidBody)
    Collider* collider; ///< Pointer to the Collider object
    btMotionState* groundMotionState; ///< Pointer to the Bullet motion state (btMotionState)
public:
    RigidGround(GroundCollider* collider);

    btRigidBody* GetGroundBody() const { return groundBody; }

};