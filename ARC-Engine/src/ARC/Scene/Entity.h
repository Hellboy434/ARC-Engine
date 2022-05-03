#pragma once
#include "ARC\Objects\Ecs.h"
#include "Component.h"

namespace ARC { class CScene; }

namespace ARC {
	class CEntity
	{
	public:
		CEntity() =default;
		CEntity(EntityID pHandle, CScene* pScene);
		CEntity(const CEntity&) = default;

		EntityID GetID() const { return m_Entity; }

		template<typename T>
		[[nodiscard]] inline bool HasComponent() const { 
			ARC_CORE_ASSERT(m_Scene);
			return m_Scene && m_Scene->GetManager().HasComponent<T>(m_Entity);
		}
		template<typename T>
		[[nodiscard]] inline T& GetComponent() {
			ARC_CORE_ASSERT(m_Scene && HasComponent<T>());
			return m_Scene->GetManager().GetComponent<T>(m_Entity);
		}
		template<typename T, typename ... Ts>
		inline T& AddComponent(Ts&&... pArgs) {
			ARC_CORE_ASSERT(m_Scene)
			auto& comp = m_Scene->GetManager().AddComponent<T>(m_Entity, std::forward<Ts>(pArgs)...);
			comp.OnConstruct(this);
			return comp;
		}
		template<typename T>
		inline void RemoveComponent() {
			ARC_CORE_ASSERT(m_Scene && HasComponent<T>())
			return m_Scene->GetManager().RemoveComponent<T>(m_Entity);
		}
		
		template<typename T>
		[[nodiscard]] inline bool HasTag() const { 
			ARC_CORE_ASSERT(m_Scene);
			return m_Scene && m_Scene->GetManager().HasTag<T>(m_Entity);
		}
		template<typename T>
		inline void AddTag() {
			ARC_CORE_ASSERT(m_Scene)
			m_Scene->GetManager().AddTag<T>(m_Entity);
		}
		template<typename T>
		inline void RemoveTag() {
			ARC_CORE_ASSERT(m_Scene && HasTag<T>())
			m_Scene->GetManager().RemoveTag<T>(m_Entity);
		}
		// 0th entity is invalid
		operator bool() const { return m_Entity; }
		bool operator == (const CEntity _) const { return m_Entity == _.m_Entity && m_Scene == _.m_Scene; }
		bool operator != (const CEntity _) const { return !(*this == _); }
	protected:

	private:
		EntityID m_Entity{0};
		CScene* m_Scene = nullptr;
	};
}