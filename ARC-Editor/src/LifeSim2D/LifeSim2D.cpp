#include "PCH/arc_pch.h"
#include "LifeSim2D.h"
#include "ARC/Scene/Scene.h"
#include "imgui/imgui.h"
#include "ARC/Scene/Entity.h"
#include "ARC/Scene/BasicComponents.h"
#include "ARC/Scene/ColliderComponents.h"
#include "ARC/Scene/SceneRegistry.h"
#include "ARC/Helpers/Random.h"

namespace ARC {
	// 2d only
	static float Time_Multiplier = 1.f;
	static float TimeElapsed_World = 0.0;
	static float TimeElapsed_ThisFrame = 0.0;
	static bool bPause = false;

	static FVec3 FindClosestPointOnLine(FVec3 pLB, FVec3 pLE, FVec3 pP)
	{
		float LineLength = SMath::Dist(pLB, pLE);
		FVec3 Vector = pP - pLB;
		FVec3 LineDirection = (pLE - pLB) / LineLength;

		FVec3 tmp(Vector * LineDirection);
		float Distance = tmp.x + tmp.y;

		if (Distance <= 0.f) return pLB;
		if (Distance >= LineLength) return pLE;
		return pLB + LineDirection * Distance;
	}

	void CLifeSim2D::OnAttach(TRef<CScene>& pActiveScene)
	{
		m_ActiveScene = pActiveScene;
		auto cam = m_ActiveScene->CreateEntity("Camera");
		cam.AddComponent<CCameraComponent>().bPrimary = true;
		cam.AddComponent<CTransform2DComponent>();

		auto tex0 = CTexture2D::Create("assets/textures/Rocksquare.png");
		auto tex1 = CTexture2D::Create("assets/textures/circle-64.png");

		std::vector<FVec3> verts = {{-.5,.5, 0}, {.5,.5,0}, {.5,-.5,0}, {-.5,-.5,0}};

		{
			auto electron = m_ActiveScene->CreateEntity("Atom1");
			auto& _ = electron.AddComponent<CTransform2DComponent>().Transform.Location = FVec3(-10, 0, 0);
			
			electron.AddComponent<CSpriteRendererComponent>(FColor4::Yellow(), tex0, FVec2::OneVector());
			electron.AddComponent<CVelocityComponent>(FVec3(1.f, 0.f, 0.f));
			electron.AddComponent<CAngularVelocityComponent>(FVec3(1.f, 0.f, 0.f));
			electron.AddComponent<CMassComponent>();
			electron.AddComponent<CPolyColliderComponent>().Vertices = verts;
		}
		{
			auto electron = m_ActiveScene->CreateEntity("Atom2");
			auto& _ = electron.AddComponent<CTransform2DComponent>().Transform.Location = FVec3(10, 0, 0);

			electron.AddComponent<CSpriteRendererComponent>(FColor4::Blue(), tex0, FVec2::OneVector());
			electron.AddComponent<CVelocityComponent>(FVec3(-1.f, 0.f, 0.f));
			electron.AddComponent<CAngularVelocityComponent>(FVec3(1.f, 0.f, 0.f));
			electron.AddComponent<CMassComponent>();
			electron.AddComponent<CPolyColliderComponent>().Vertices = verts;
		}
		{
			auto electron = m_ActiveScene->CreateEntity("Atom3");
			auto& _ = electron.AddComponent<CTransform2DComponent>().Transform.Location = FVec3(0, -10, 0);

			electron.AddComponent<CSpriteRendererComponent>(FColor4::Green(), tex1, FVec2::OneVector());
			electron.AddComponent<CVelocityComponent>(FVec3(0.f, 1.f, 0.f));
			electron.AddComponent<CAngularVelocityComponent>(FVec3(1.f, 0.f, 0.f));
			electron.AddComponent<CMassComponent>();
			electron.AddComponent<CCircleColliderComponent>().Radius = 0.5;
		}
	}

	void CLifeSim2D::OnDetach()
	{
	}

	void CLifeSim2D::OnUpdate(float pDeltaTime)
	{
		if (bPause) return;
		static const FVec3 Mask = FVec3(1, 1, 0);

		TimeElapsed_ThisFrame = pDeltaTime * Time_Multiplier;
		TimeElapsed_World += TimeElapsed_ThisFrame;
	}

	void CLifeSim2D::OnGuiRender()
	{
		ImGui::Begin("Atomic");
		ImGui::DragFloat("Speed", &Time_Multiplier);
		ImGui::DragFloat("Time Elapsed(World)", &TimeElapsed_World);
		ImGui::Checkbox("Pause", &bPause);
		ImGui::End();
	}

	void CLifeSim2D::OnEvent(CEvent& pEvent)
	{
	}
}