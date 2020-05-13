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

struct Enemy {};

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
		//** Example: Query entities.
		EntityArchetype enemyQuery = getEntityManager()->getArchetype<Enemy>();
		const std::set<Entity>& enemyEntities = getEntityManager()->getEntityQuery(enemyQuery);

		std::cout << "Counted " << getEntities().size() << " total amount of entities. Of which " << enemyEntities.size() << " are enemies." << std::endl;
	}
};

// -----Main-----
int main(int argc, char** argv)
{
	EntityManager entityManager;

	//** Example: register a component.
	entityManager.registerComponent<Enemy>();

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

		// Make 50% enemies.
		if (i % 2 == 0)
			entityManager.addComponent<Enemy>(entity);
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