#include "SimulationApp.hpp"

#include "glm/gtx/string_cast.hpp"

#include "ClothSceneNode.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/components/CameraComponent.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/debug/AxisNode.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/shaders/PhongShader.hpp"

namespace GLOO {
SimulationApp::SimulationApp(const std::string& app_name,
                             glm::ivec2 window_size,
                             IntegratorType integrator_type,
                             float integration_step)
    : Application(app_name, window_size),
      integrator_type_(integrator_type),
      integration_step_(integration_step) {
}

void SimulationApp::SetupScene() {
  SceneNode& root = scene_->GetRootNode();

  auto simple_ptr = make_unique<SimpleSceneNode>(integrator_type_, integration_step_);
  root.AddChild(std::move(simple_ptr));

  auto pendulum_ptr = make_unique<PendulumSceneNode>(integrator_type_, integration_step_);
  root.AddChild(std::move(pendulum_ptr));

  auto cloth_ptr = make_unique<ClothSceneNode>(integrator_type_, integration_step_);
  root.AddChild(std::move(cloth_ptr));

  auto camera_node = make_unique<ArcBallCameraNode>(45.f, 0.75f, 5.0f);
  scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
  root.AddChild(std::move(camera_node));

  root.AddChild(make_unique<AxisNode>('A'));

  auto ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(glm::vec3(0.7f));
  root.CreateComponent<LightComponent>(ambient_light);

  auto point_light = std::make_shared<PointLight>();
  point_light->SetDiffuseColor(glm::vec3(0.9f, 0.9f, 0.9f));
  point_light->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
  point_light->SetAttenuation(glm::vec3(1.0f, 0.09f, 0.032f));
  auto point_light_node = make_unique<SceneNode>();
  point_light_node->CreateComponent<LightComponent>(point_light);
  point_light_node->GetTransform().SetPosition(glm::vec3(0.0f, 2.0f, 4.f));
  root.AddChild(std::move(point_light_node));
}
}  // namespace GLOO
