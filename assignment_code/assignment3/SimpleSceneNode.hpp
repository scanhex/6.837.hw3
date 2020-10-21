#pragma once

#include "ODESceneNode.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "SimpleSystem.hpp"
#include "IntegratorType.hpp"
#include "gloo/shaders/PhongShader.hpp"

#include <gloo/VertexObject.hpp>
#include <gloo/components/RenderingComponent.hpp>
#include <gloo/components/ShadingComponent.hpp>
#include <gloo/debug/PrimitiveFactory.hpp>
#include <vector>

namespace GLOO {

class SimpleSceneNode : public ODESceneNode<SimpleSystem, ParticleState> {
  void onUpdate() {
    this->sphere_->GetTransform().SetPosition(state_.positions[0]);
  }
  SceneNode *sphere_;
  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<PhongShader> shader_;

public:
  SimpleSceneNode(IntegratorType type, float integration_step)
      : ODESceneNode(ParticleState{{{3, 0, 0}}, {{0,0,0}}}, type, integration_step) {
    shader_ = std::make_shared<PhongShader>();
    auto sphere_ptr = make_unique<SceneNode>();
    sphere_ = sphere_ptr.get();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.5, 50, 50);
    sphere_ptr->CreateComponent<ShadingComponent>(shader_);
    sphere_ptr->CreateComponent<RenderingComponent>(sphere_mesh_);
    this->AddChild(std::move(sphere_ptr));
  }
};
}

