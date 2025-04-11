#pragma once
#include <vector>

#include "ModelAPI/ModelAPI.h"
#include "PhysicsManager.h"
#include "ModelAPI/ModelInfo.h"
#include <d3d11.h>

class Scene
{
private:
	PhysicsManager physiqueManager;

public:
	virtual ~Scene() = default;

	virtual void AddModel(const char path[], const char texture_path[]) = 0;
	virtual void RemoveModel(int IDX) = 0;
	virtual int GetModelCount() = 0;
	virtual ModelInfo* GetModelInfoAtIdx(int IDX) = 0;
	virtual std::vector<ModelInfo*> GetModelsInfo() = 0;

	virtual void AddPostProccessPass(const char path[], int ScreenWidth, int ScreenHeight) = 0;
	virtual void RemovePostProccessPassAtIdx(int Idx) = 0;

	virtual uint32_t GetRenderTarget() = 0;

	void SetDxDeviceContext(ID3D11Device* dxDevice, ID3D11DeviceContext* d3d11DevCon);

	virtual void Draw() = 0;

	PhysicsManager GetPhysics()
	{
		return physiqueManager;
	};

	void CreatePhysics()
	{
		physiqueManager.initPhysics();
	}

	void AddPhysicsBody(ModelInfo* info)
	{
		MeshCollider* meshcollider = new MeshCollider(info->vertexs, info->indicies);
		RigidBody* meshbody = new RigidBody(meshcollider, 1, btVector3(0, 3, 0));
		physiqueManager.addRigidBody(meshbody);
		/*BoxCollider* box = new BoxCollider(btVector3(1, 1, 1));
		RigidBody* boxbody = new RigidBody(box,1,btVector3(0,10,0));
		physiqueManager.addRigidBody(boxbody);*/
	}

	void AddPhysicsGround()
	{
		GroundCollider* ground = new GroundCollider();
		RigidGround* groundbody = new RigidGround(ground);
		physiqueManager.addGroundBody(groundbody);
	}

	void UpdatePhysics(float deltaTime)
	{
		physiqueManager.updatePhysics(deltaTime);
	}

	bool GetCollisionCheck(btRigidBody& body1, btRigidBody& body2)
	{
		// Récupérer le monde physique
		btDiscreteDynamicsWorld* world = physiqueManager.getWorld();
		if (!world)
			return false; // Vérifier que le monde est valide

		// Récupérer le dispatcher du monde physique (il gère les collisions)
		btDispatcher* dispatcher = world->getDispatcher();

		// Parcourir tous les contacts actifs
		int numManifolds = dispatcher->getNumManifolds();
		for (int i = 0; i < numManifolds; i++)
		{
			btPersistentManifold* contactManifold = dispatcher->getManifoldByIndexInternal(i);

			const btCollisionObject* objA = contactManifold->getBody0();
			const btCollisionObject* objB = contactManifold->getBody1();

			// Vérifier si les objets en collision sont bien body1 et body2
			if ((objA == &body1 && objB == &body2) || (objA == &body2 && objB == &body1))
			{
				int numContacts = contactManifold->getNumContacts();
				for (int j = 0; j < numContacts; j++)
				{
					btManifoldPoint& pt = contactManifold->getContactPoint(j);

					if (pt.getDistance() < -0.001f)
					{
						// Collision réelle uniquement si distance < 0
						return true;
					}
				}
			}
		}

		return false; // Aucune collision détectée
	}

	void DestroyPhysics()
	{
		physiqueManager.destroyPhysics();
	}
};
