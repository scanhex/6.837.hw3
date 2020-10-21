#pragma once

#include "gloo/Application.hpp"

#include "IntegratorType.hpp"
#include "ODESceneNode.hpp"
#include "SimpleSystem.hpp"
#include "SimpleSceneNode.hpp"
#include "PendulumSceneNode.hpp"

namespace GLOO {
class SimulationApp : public Application {
 public:
  SimulationApp(const std::string& app_name,
                glm::ivec2 window_size,
                IntegratorType integrator_type,
                float integration_step);
  void SetupScene() override;

 private:
  IntegratorType integrator_type_;
  float integration_step_;
  SimpleSceneNode* simple_;
  PendulumSceneNode* pendulum_;
//  ODESceneNode<PendulumSystem, ParticleState> pendulum_;
//  ODESceneNode<ClothSystem, ParticleState> cloth_;
};
}  // namespace GLOO