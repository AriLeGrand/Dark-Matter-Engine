#include "RigidBody.h"

RigidBody::RigidBody(Collider* collider, float mass, btVector3 position) : collider(collider)
{
	btVector3 localInertia(0, 0, 0);
	if (mass > 0)
		collider->GetShape()->calculateLocalInertia(mass, localInertia);

	try
	{
		motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), position));
		console.Log("RigidBody motion state created", API::PHYSICS, LEVEL::INFO);
	}
	catch (const std::exception& e)
	{
		console.Log("Failed to create motion state: " + std::string(e.what()), API::PHYSICS, LEVEL::ERRORS);
	}

	btRigidBody::btRigidBodyConstructionInfo info(mass, motionState, collider->GetShape(), localInertia);
	body = new btRigidBody(info);
	console.Log("RigidBody created", API::PHYSICS, LEVEL::INFO);
}

void RigidBody::ApplyForce(const btVector3& force)
{
	body->activate();
	body->applyCentralForce(force);
	console.Log("Applied force to RigidBody", API::PHYSICS, LEVEL::INFO);
}

void RigidBody::SetVelocity(const btVector3& velocity)
{
	body->activate();
	body->setLinearVelocity(velocity);
	console.Log("Set RigidBody velocity", API::PHYSICS, LEVEL::INFO);
}

void RigidBody::DoImpulse(const btVector3& impulse)
{
	body->activate();
	body->applyCentralImpulse(impulse);
	console.Log("Applied impulse to RigidBody", API::PHYSICS, LEVEL::INFO);
}

void RigidBody::SetPosition(const btVector3& position)
{
	btTransform transform;
	body->getMotionState()->getWorldTransform(transform);
	transform.setOrigin(position);
	body->setWorldTransform(transform);
	body->getMotionState()->setWorldTransform(transform);
	body->activate();
	console.Log("RigidBody position updated", API::PHYSICS, LEVEL::INFO);
}

void RigidBody::SetAngularVelocity(const btVector3& angularVelocity)
{
	body->activate();
	body->setAngularVelocity(angularVelocity);
	console.Log("Set RigidBody angular velocity", API::PHYSICS, LEVEL::INFO);
}

void RigidBody::ApplyTorque(const btVector3& torque)
{
	body->activate();
	body->applyTorque(torque);
	console.Log("Applied torque to RigidBody", API::PHYSICS, LEVEL::INFO);
}

void RigidBody::SetRotation(const btQuaternion& rotation)
{
	btTransform transform;
	body->getMotionState()->getWorldTransform(transform);
	transform.setRotation(rotation); // Applique la rotation via le quaternion

	body->setWorldTransform(transform);
	body->getMotionState()->setWorldTransform(transform);
	body->setInterpolationWorldTransform(transform); // Pour éviter des effets de glitch
	body->activate();
	console.Log("RigidBody rotation updated", API::PHYSICS, LEVEL::INFO);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

RigidGround::RigidGround(GroundCollider* collider)
{
	groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo groundInfo(0, groundMotionState, collider->GetShape());
	groundBody = new btRigidBody(groundInfo);

	console.Log("Ground body created", API::PHYSICS, LEVEL::INFO);
}
