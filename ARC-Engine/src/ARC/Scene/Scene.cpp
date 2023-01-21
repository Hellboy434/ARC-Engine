#include "arc_pch.h"
#include "Scene.h"
#include "ARC\Renderer\Renderer2D.h"
#include "ARC\Objects\Primitive2D.h"
#include "Utils\MPL\Interface.hpp"

namespace ARC {

	CScene::CScene()
	{
		CScene::SetupComponent<CTransform2DComponent>();
		CScene::SetupComponent<CSpriteRendererComponent>();
		CScene::SetupComponent<CCameraComponent>();
		CScene::SetupComponent<CNativeScriptComponent>();
		
		CScene::SetupComponent<CNetForceComponent>();
		CScene::SetupComponent<CElectricSignComponent>();
		CScene::SetupComponent<CMassComponent>();
		CScene::SetupComponent<CVelocityComponent>();
	}

	CScene::~CScene()
	{

	}

	CEntity CScene::CreateEntity(const TString& pName)
	{
		CEntity entity(m_Manager.create(), this);

		entity.AddComponent<CNameComponent>(pName);

		return entity;
	}

	void CScene::RemoveEntity(CEntity Entity)
	{
		m_Manager.destroy(Entity.GetID());
		Entity.OnKill();
	}

	void CScene::OnUpdate(float DeltaTime)
	{
		m_Manager.view<CNativeScriptComponent>().each([=](auto entity, auto& nativeScriptcomp)
			{
				if (!nativeScriptcomp.Controller)
				{
					nativeScriptcomp.Controller = nativeScriptcomp.InstantiateDelegate();
					nativeScriptcomp.Controller->Setup(nativeScriptcomp.OwningEntity);
					nativeScriptcomp.Controller->OnCreate();
				}

				nativeScriptcomp.Controller->OnUpdate(DeltaTime);
			});
		
		CCamera* mainCam = nullptr;
		FTransform2D* camTransform = nullptr;
		
		m_Manager.view<CTransform2DComponent, CCameraComponent>().each([&](auto entity, auto& transformComp, auto& cameraComp)
			{
				if (cameraComp.bPrimary)
				{
					mainCam = &cameraComp.Camera;
					camTransform = &transformComp.Transform;
					return;
				}
			});

		if (mainCam)
		{
			CRenderer2D::BeginScene(*mainCam, *camTransform);

			if (m_Manager.view<CCameraComponent>().size()!=1)
			{
				static CPrimitive2D Quad;
				CRenderer2D::DrawQuad(Quad);
			}

			m_Manager.group<CTransform2DComponent>(entt::get<CSpriteRendererComponent>).each([](auto entity, auto& transformComp, auto& spriteRendererComp)
				{
					static CPrimitive2D Quad;
					Quad.Color = spriteRendererComp.Color;
					Quad.Texture = spriteRendererComp.Texture;
					Quad.TextureScaling = spriteRendererComp.TextureScaling;
					Quad.Transform = transformComp;
					CRenderer2D::DrawQuad(Quad);
				});

			CRenderer2D::EndScene();
		}

		m_Manager.view<CTransform2DComponent, CVelocityComponent>().each([](auto entity, auto& transformComp, auto& velocityComp)
			{
				transformComp.Transform.Location += velocityComp.Velocity;
			});
	}

	void CScene::OnViewportResize(TVec2<uint32_t> pNewSize)
	{
		m_ViewportSize = pNewSize;

		m_Manager.view<CCameraComponent>().each([=](auto entity, auto& cameraComp)
			{
				if (!cameraComp.bFixedAspectRatio)
				{
					cameraComp.Camera.SetViewportSize(pNewSize);
				}
			});
	}

	void CScene::SerializeEntity(YAML::Emitter& pOut, CEntity pEntity)
	{
		pOut << YAML::BeginMap;
		pOut << YAML::Key << "Entity" << YAML::Value << "1038845309209"; // @TODO

		pEntity.Serialize(false, pOut); // @TODO notify if binary serialization

		pOut << YAML::EndMap;
	}

	void CScene::SerializeToText(const TString& pFilepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;

		m_Manager.each([&](const TEntityID pEID)
			{
				CEntity entity = { pEID, this };
				if (!entity) return;

				CScene::SerializeEntity(out, entity);			
			});

		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(pFilepath.c_str());
		fout << out.c_str();
	}

	void CScene::SerializeToBinary(const TString& pFilepath)
	{
		
	}

	bool CScene::DeserializeFromText(const TString& pFilepath)
	{
		YAML::Node data = YAML::LoadFile(pFilepath.c_str());
		if (!data["Scene"]) return false;

		TString SceneName = data["Scene"].as<TString>();

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				CEntity deserializedEntity = CreateEntity();
				
				//bool bFirst = true;
				//for (auto cmp : entity)
				//{
				//	if (bFirst) { bFirst = false; continue; }
				//	using namespace entt::literals;
				//	entt::resolve(entt::hashed_string::value(cmp.as<std::string>().c_str())).func("assign"_hs).invoke(
				//		{},
				//		deserializedEntity
				//		entt::forward_as_meta(cmp)
				//	);
				//}
				
				//MPL::forTypes<MyComponents>([&](auto t) {
				//	using tCompType = ECS_TYPE(t);
				//
				//	if constexpr (tCompType::Flags & ECF::Serializable) {
				//		if (auto comp = entity[CComponentTraits::GetName<tCompType>()])
				//		{
				//			tCompType* component = nullptr;
				//			if constexpr (!(tCompType::Flags & ECF::AutoAddToAll))
				//				component = &deserializedEntity.AddComponent<tCompType>();
				//			else
				//				component = &deserializedEntity.GetComponent<tCompType>();
				//			
				//			component->Deserialize(comp);
				//		}
				//	}
				//	});

			}
		}
		return true;
	}

	bool CScene::DeserializeFromBinary(const TString& pFilepath)
	{
		return false;
	}
}