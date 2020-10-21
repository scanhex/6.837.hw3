#pragma once
#include "ForwardEulerIntegrator.hpp"
#include "TrapezoidalIntegrator.hpp"
#include "IntegratorBase.hpp"
#include "IntegratorType.hpp"
#include "RK4Integrator.hpp"
#include "gloo/SceneNode.hpp"

namespace GLOO {
template<typename TSystem, typename TState>
class ODESceneNode : public SceneNode {
public:

  ODESceneNode(TSystem system, TState state, IntegratorType integrator_type, float integration_step)
    : system_(system), state_(state)
  {
    if (integrator_type == IntegratorType::Euler)
      integrator_ = make_unique<ForwardEulerIntegrator<TSystem, TState>>(integration_step);
    else if (integrator_type == IntegratorType::Trapezoidal)
      integrator_ = make_unique<TrapezoidalIntegrator<TSystem, TState>>(integration_step);
    else {
      assert(integrator_type == IntegratorType::RK4);
      integrator_ = make_unique<RK4Integrator<TSystem, TState>>(integration_step);
    }
  }

  ODESceneNode(TState state, IntegratorType integrator_type, float integration_step)
      : ODESceneNode(TSystem(), state, integrator_type, integration_step) {}
  virtual void onUpdate() = 0;
  void Update(double delta_time) override {
    state_ = integrator_->Integrate(system_, state_, cur_time_, cur_time_ + delta_time);
    cur_time_ += delta_time;
    onUpdate();
  }
  std::unique_ptr<IntegratorBase<TSystem, TState>> integrator_;
  TSystem system_;
  TState state_;
  double cur_time_ = 0;
};
}
