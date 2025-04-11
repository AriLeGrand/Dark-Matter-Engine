#pragma once
#include <iostream>
#include <Bullet/btBulletDynamicsCommon.h>
#include <unordered_set>
#include "ModelAPI/ModelLoader.h"
#include "ConsoleManager.h"




/// <summary>
/// Class responsible for managing colliders
/// </summary>
class Collider {
protected:
    btCollisionShape* shape; ///< Pointer to the collider's shape

public:

    /// <summary>
    /// getter to retrieve the shape
    /// </summary>
    /// <returns>the shape of the collider</returns>
    virtual btCollisionShape* GetShape() const = 0;
};


/// <summary>
/// Class of the box collider
/// </summary>
class BoxCollider : public Collider {
public:
    /// <summary>
    /// create a box shape
    /// </summary>
    /// <param name="halfSize">btVector3 : half of the size of the box </param>
    BoxCollider(const btVector3& halfSize);


    /// <summary>
    /// getter to retrieve the box shape
    /// </summary>
    /// <returns>the box shape</returns>
    btCollisionShape* GetShape() const override { return shape; }
};


/// <summary>
/// Class of the sphere collider
/// </summary>
class SphereCollider : public Collider {
public:

    SphereCollider(float radius);


    btCollisionShape* GetShape() const override { return shape; }
};


/// <summary>
/// Class of the capsule collider
/// </summary>
class CapsuleCollider : public Collider{
public:
    CapsuleCollider(float radius, float height);
    
    btCollisionShape* GetShape() const override { return shape; }
};


/// <summary>
/// Class for handling mesh colliders
/// </summary>
class MeshCollider : public Collider {
public:

    /// <summary>
    /// Constructor to create a mesh collider
    /// </summary>
    /// <param name="model">Model_loader : The loaded mesh data</param>
    MeshCollider(std::vector<float> vertices, std::vector<unsigned int> indices);

    /// <summary>
    /// Getter to retrieve the mesh shape
    /// </summary>
    /// <returns>Pointer to the shape</returns>
    btCollisionShape* GetShape() const override;
};


/// <summary>
/// Class of the ground collider
/// </summary>
class GroundCollider : public Collider
{
public:
    GroundCollider();

    btCollisionShape* GetShape() const override { return shape; }
};
