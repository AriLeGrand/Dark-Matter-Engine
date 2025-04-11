#include "PhysicsManager.h"
#include "RigidBody.h"

PhysicsManager::PhysicsManager() : world(nullptr), broadphase(nullptr), collisionConfig(nullptr), dispatcher(nullptr), solver(nullptr)
{
	console.Log("PhysicsManager initialized", API::PHYSICS, LEVEL::INFO);
}

void PhysicsManager::initPhysics()
{
	console.Log("Initializing physics engine", API::PHYSICS, LEVEL::INFO);

	broadphase = new btDbvtBroadphase();
	collisionConfig = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	world->setGravity(btVector3(0, gravity, 0));

	console.Log("Physics engine initialized successfully", API::PHYSICS, LEVEL::SUCCESS);
}

void PhysicsManager::updatePhysics(float deltaTime)
{
	if (world != nullptr)
	{
		world->stepSimulation(deltaTime, 10);
	}
	else
	{
		console.Log("World is null. Cannot update physics.", API::PHYSICS, LEVEL::ERRORS);
	}
}

void PhysicsManager::destroyPhysics()
{
	if (world)
	{
		console.Log("Destroying physics engine", API::PHYSICS, LEVEL::INFO);

		if (ground != nullptr)
		{
			world->removeRigidBody(ground);
			delete ground;
			console.Log("Ground body removed", API::PHYSICS, LEVEL::INFO);
		}

		// Delete all physical objects
		for (auto body : rigidBodies)
		{
			world->removeRigidBody(body->GetBulletBody());
			delete body;
			console.Log("RigidBody removed", API::PHYSICS, LEVEL::INFO);
		}
		rigidBodies.clear();

		// Delete Bullet Physics
		delete world;
		delete solver;
		delete dispatcher;
		delete collisionConfig;
		delete broadphase;

		console.Log("Physics engine destroyed", API::PHYSICS, LEVEL::SUCCESS);

		ground = nullptr;
		world = nullptr;
		solver = nullptr;
		dispatcher = nullptr;
		collisionConfig = nullptr;
		broadphase = nullptr;
	}
	else
	{
		console.Log("World is null. Cannot destroy physics.", API::PHYSICS, LEVEL::ERRORS);
	}
}

void PhysicsManager::addRigidBody(RigidBody* body)
{
	if (world)
	{
		rigidBodies.push_back(body);
		world->addRigidBody(body->GetBulletBody());
		console.Log("RigidBody added", API::PHYSICS, LEVEL::INFO);
	}
	else
	{
		console.Log("World is null. Cannot add RigidBody.", API::PHYSICS, LEVEL::ERRORS);
	}
}

void PhysicsManager::addGroundBody(RigidGround* body)
{
	if (world)
	{
		ground = body->GetGroundBody();
		world->addRigidBody(body->GetGroundBody());
		console.Log("Ground body added", API::PHYSICS, LEVEL::INFO);
	}
	else
	{
		console.Log("World is null. Cannot add GroundBody.", API::PHYSICS, LEVEL::ERRORS);
	}
}

void PhysicsManager::removeRigidBody(RigidBody* body)
{
	if (world)
	{
		world->removeRigidBody(body->GetBulletBody());
		rigidBodies.erase(std::remove(rigidBodies.begin(), rigidBodies.end(), body), rigidBodies.end());
		console.Log("RigidBody removed", API::PHYSICS, LEVEL::INFO);
	}
	else
	{
		console.Log("World is null. Cannot remove RigidBody.", API::PHYSICS, LEVEL::ERRORS);
	}
}

void PhysicsManager::removeGroundBody(RigidGround* body)
{
	if (world)
	{
		world->removeRigidBody(body->GetGroundBody());
		console.Log("Ground body removed", API::PHYSICS, LEVEL::INFO);
	}
	else
	{
		console.Log("World is null. Cannot remove GroundBody.", API::PHYSICS, LEVEL::ERRORS);
	}
}
