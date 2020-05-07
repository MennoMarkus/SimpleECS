#pragma once
#include <array>
#include <unordered_map>
#include <memory>
#include <typeinfo>

#include "ecs/Entity.h"
#include "ecs/ComponentDataArray.h"

class ISystemBase;

class EntityManager
{
private:
	size_t m_activeEntityCount = 0;

	// Each entry indicates if that index is an avialable ID.
	std::array<bool, MAX_ENTITY_ID> m_avialableEntityIDs;

	std::array<EntityArchetype, MAX_ENTITY_ID> m_entityArchetypes;

	// Components
	ComponentType m_currentComponentTypeID = 0;
	std::unordered_map<size_t, ComponentType> m_componentTypes;
	std::unordered_map<size_t, std::unique_ptr<IComponentDataArray>> m_componentArrays;

	// Systems
	std::vector<std::unique_ptr<ISystemBase>> m_systems;

public:
	EntityManager();
	void update();

	template<typename T> void registerComponent(size_t reserveSize = 256);
	template<typename T, typename ...Args> void registerSystem(Args&& ...args);
	void registerSystem(std::unique_ptr<ISystemBase> system);

	Entity createEntity();
	void destroyEntity(Entity entity);

	EntityArchetype getArchetype(Entity entity) const;

	template<typename T> void reserveComponentCount(size_t reserveSize = 256);
	template<typename T> ComponentType getComponentType() const;

	template<typename T> T& getComponent(Entity entity);
	template<typename T> bool hasComponent(Entity entity) const;

	template<typename T> void setComponent(Entity entity, T componentData);
	template<typename T> void addComponent(Entity entity);
	template<typename T> void removeComponent(Entity entity);

	template<typename T> ComponentDataArray<T>& getComponentDataArray();

private:
	Entity getFirstAvialableEntityID() const;

	void setArchetype(Entity entity, EntityArchetype archetype);
};

#include "ecs/EntityManager.inl"