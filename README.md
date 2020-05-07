# SimpleECS
 This project demostrates a simple ECS system. The perpose of this project is to learn more about ECS and modern C++, and is not meant as a feature complete ECS system. (There are plenty of better libraries for ECS than this project.)
 
 ### Known issues
 - Inserting entities into a system while this system is running does not garanty when the the newly inserted entities will be updated by the system. Either the entity is updated this frame or next frame with no way of knowing.  
 *Potential solution: Implement the EntityCommandBuffer pattern from Unity DOTS.*
 
 ### References
 - https://austinmorlan.com/posts/entity_component_system/ 
 - https://www.gamasutra.com/blogs/TobiasStein/20171122/310172/The_EntityComponentSystem__An_awesome_gamedesign_pattern_in_C_Part_1.php
 - https://www.basgeertsema.nl/blog/2016/10/ecsos-entity-component-system-based-on-ordered-sets/
 - The Unity DOTS documentation.

### Example usage
Example can be found in Main.cpp.
```cpp
#include <vector>
#include <iostream>

#include "ecs/EntityManager.h"

// -----Components-----
struct Position
{
	float x = 0.0f;
	float y = 100.0f;
};

struct Velocity
{
	float x, y;
};

// -----Systems-----
class GravitySystem : public SystemBase<Velocity>
{
public:
	GravitySystem() : SystemBase("GravitySystem") {}

	void foreach(Entity entity, Velocity& velocity) override
	{
		velocity.y -= 9.81f;
	}
};

class GroundCollisionSystem : public SystemBase<Position, Velocity>
{
public:
	GroundCollisionSystem() : SystemBase("GroundCollisionSystem") {}

	void foreach(Entity entity, Position& position, Velocity& velocity) override
	{
		if (position.y <= 0.0f)
		{
			position.y = 0.0f;
			velocity.y = 0.0f;
		}

		if (position.x < -500.0f || position.x > 500.0f)
		{
			//** Example: Entity destruction during system update.
			getEntityManager()->destroyEntity(entity);
			std::cout << "Entity " << entity << ": destroyed!" << std::endl;
		}
	}
};

//** Example: Systems without component arguments run on all entities.
class EntityCounterSystem : public SystemBase<>
{
public:
	EntityCounterSystem() : SystemBase("EntityCounterSystem") {}

	//** Example: Override update to manually iterate over all entities that match this system.
	void update() override
	{
		std::cout << "Counted " << getEntities().size() << " total amount of entities." << std::endl;
	}
};

// -----Main-----
int main(int argc, char** argv)
{
	EntityManager entityManager;

	//** Example: register a system.
	// Systems are run in the order in that they were registered.
	entityManager.registerSystem<EntityCounterSystem>();
	entityManager.registerSystem<GravitySystem>();
	entityManager.registerSystem<GroundCollisionSystem>();

	//** Example: create and register a system using lambda.
	std::unique_ptr<ISystemBase> physicsSystem(new SystemBase<Position, Velocity>("PhysicsSystem",
		[](Entity entity, Position& position, Velocity& velocity)
		{
			std::cout << "Entity " << entity << ": position(x: " <<  position.x << ", y: " << position.y << ") -> ";
			position.x += velocity.x;
			position.y += velocity.y;
			std::cout << "position(x: " << position.x << ", y: " << position.y << ")" << std::endl;
		}
	));
	entityManager.registerSystem(std::move(physicsSystem));

	//** Example: register component.
	// Not needed here as systems will automatically register the components they run on.
	entityManager.registerComponent<Position>();
	entityManager.registerComponent<Velocity>();

	//** Example: create entities and add components.
	for (int i = 0; i < 10; i++)
	{
		Entity entity = entityManager.createEntity();
		entityManager.addComponent<Position>(entity);

		// Ititialize with a random velocity.
		entityManager.setComponent<Velocity>(entity, Velocity{ 
			((float)rand() / (float)RAND_MAX) * 100.0f, 
			((float)rand() / (float)RAND_MAX) * 100.0f 
		});
	}

	//** Example: Create/destroy entities and components.
	// Does nothing here as the entity is immediatly deleted.
	Entity tempEntity = entityManager.createEntity();
	entityManager.setComponent(tempEntity, Position{100.0f, 100.0f});
	entityManager.removeComponent<Position>(tempEntity);
	entityManager.destroyEntity(tempEntity);

	//** Example: update loop.
	for (int i = 0; i < 10; i++)
	{
		std::cout << "\n-----Frame: " << i << "-----" << std::endl;
		entityManager.update();
	}

	return 0;
}
```
