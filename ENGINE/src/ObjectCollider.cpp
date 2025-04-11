#include "ObjectCollider.h"

BoxCollider::BoxCollider(const btVector3& halfSize)
{
	shape = new btBoxShape(halfSize);
	console.Log("Box collider created", API::PHYSICS, LEVEL::INFO);
}

/// <summary>
/// Constructor: Creates a sphere collider.
/// </summary>
SphereCollider::SphereCollider(float radius)
{
	shape = new btSphereShape(radius);
	console.Log("Sphere collider created", API::PHYSICS, LEVEL::INFO);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Constructor: Creates a capsule collider.
/// </summary>
CapsuleCollider::CapsuleCollider(float radius, float height)
{
	shape = new btCapsuleShape(radius, height);
	console.Log("Capsule collider created", API::PHYSICS, LEVEL::INFO);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary>
/// Constructor: Converts a Model_loader into a Bullet physics mesh collider.
/// </summary>
MeshCollider::MeshCollider(std::vector<float> vertices, std::vector<unsigned int> indices)
{
	// Vérification des données
	if (vertices.empty() || indices.empty())
	{
		throw std::runtime_error("Invalid vertices or indices data");
	}

	// Création de la forme convexe
	auto* convexShape = new btConvexHullShape();

	// Ajout des sommets au Convex Hull
	std::unordered_set<size_t> uniqueVertices; // Pour éviter les doublons

	for (size_t i = 0; i < indices.size(); i++)
	{
		size_t index = indices[i] * 3; // Chaque sommet a 3 valeurs (x, y, z)

		if (uniqueVertices.find(index) == uniqueVertices.end())
		{
			uniqueVertices.insert(index);

			btVector3 vertex(vertices.at(index), vertices.at(index + 1), vertices.at(index + 2));

			convexShape->addPoint(vertex, false); // "false" = évite recalculations à chaque ajout
		}
	}

	// Mise à jour de la bounding box
	convexShape->recalcLocalAabb();

	// Assigne la shape à l'objet
	shape = convexShape;
	console.Log("Mesh collider created with convex shape", API::PHYSICS, LEVEL::INFO);
}

/// <summary>
/// Returns the collision shape
/// </summary>
btCollisionShape* MeshCollider::GetShape() const
{
	return shape;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

GroundCollider::GroundCollider()
{
	shape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
	console.Log("Ground collider created", API::PHYSICS, LEVEL::INFO);
}
