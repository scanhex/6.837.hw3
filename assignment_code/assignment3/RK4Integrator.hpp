#pragma once

#include "IntegratorBase.hpp"

namespace GLOO {
template <class TSystem, class TState>
class RK4Integrator : public IntegratorBase<TSystem, TState> {
public:
  using IntegratorBase<TSystem, TState>::IntegratorBase;

protected:
  TState IntegrateStep(const TSystem& system,
                       const TState& state,
                       float start_time,
                       float step) const override {
    auto k1 = system.ComputeTimeDerivative(state, start_time);
    auto k2 = system.ComputeTimeDerivative(state + k1 * (step / 2), start_time + step / 2);
    auto k3 = system.ComputeTimeDerivative(state + k2 * (step / 2), start_time + step / 2);
    auto k4 = system.ComputeTimeDerivative(state + k3 * step, start_time + step);
    return state + (step / 6) * (k1 + 2 * k2 + 2 * k3 + k4);
  }
};
}  // namespace GLOO