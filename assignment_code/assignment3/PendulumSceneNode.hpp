#pragma once

#include "ODESceneNode.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "SimpleSystem.hpp"
#include "PendulumSystem.hpp"
#include "IntegratorType.hpp"
#include "gloo/shaders/PhongShader.hpp"

#include <gloo/VertexObject.hpp>
#include <gloo/components/RenderingComponent.hpp>
#include <gloo/components/ShadingComponent.hpp>
#include <gloo/debug/PrimitiveFactory.hpp>
#include <vector>

namespace GLOO {

const ParticleState initial_state{{{0, 0, 0}, {0, 0, -1}, {0, 1, -2}, {0, -1, -3}, {0, 0, -4}}};
const std::vector<float> coefs{1, 1, 1, 1};
const std::vector<float> masses{1, 1, 1, 1, 1};

class PendulumSceneNode : public ODESceneNode<PendulumSystem, ParticleState> {
  void onUpdate() override {
    for(size_t i=0;i<spheres_.size();++i)
      spheres_[i]->GetTransform().SetPosition(state_.positions[i]);
  }
  std::vector<SceneNode*> spheres_;
  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<PhongShader> shader_;

public:
  PendulumSceneNode(IntegratorType type, float integration_step)
      : ODESceneNode(initial_state, type, integration_step) {
    shader_ = std::make_shared<PhongShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.5, 50, 50);
    spheres_.resize(masses.size());
    for(size_t i=0;i<spheres_.size();++i) {
      auto ptr = make_unique<SceneNode>();
      ptr->CreateComponent<ShadingComponent>(shader_);
      ptr->CreateComponent<RenderingComponent>(sphere_mesh_);
      ptr->GetTransform().SetScale(glm::vec3(glm::sqrt(masses[i])));
      spheres_[i] = ptr.get();
      this->AddChild(std::move(ptr));
    }
    for (float mass : masses)
      system_.AddParticle(mass, 0.1);
    system_.setFix(0, true);
    for (size_t i = 0; i + 1 < masses.size(); ++i)
      system_.AddSpring(i, i + 1, 1, state_);
  }
};
}

