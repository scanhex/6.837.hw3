#pragma once

#include "ParticleSystemBase.hpp"

namespace GLOO {
template <class TSystem, class TState>
class IntegratorBase {
public:
  IntegratorBase(float integration_step) : integration_step_(integration_step) {}
  virtual ~IntegratorBase() {
  }
public:
  TState Integrate(const TSystem& system,
                             const TState& state,
                             float start_time,
                             float end_time) const {
    assert(end_time >= start_time);
    auto res = state;
    while(end_time - start_time > this->integration_step_) {
      res = IntegrateStep(system, res, start_time, this->integration_step_);
      start_time += this->integration_step_;
    }
    res = IntegrateStep(system, res, start_time, end_time - start_time);
    return res;
  }

protected:
  virtual TState IntegrateStep(const TSystem& system,
                           const TState& state,
                           float start_time,
                           float dt) const = 0;

  float integration_step_;
};
}  // namespace GLOO