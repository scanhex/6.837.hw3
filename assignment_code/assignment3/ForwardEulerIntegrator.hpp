#pragma once

#include "IntegratorBase.hpp"

namespace GLOO {
template <class TSystem, class TState>
class ForwardEulerIntegrator : public IntegratorBase<TSystem, TState> {
public:
  using IntegratorBase<TSystem, TState>::IntegratorBase;

protected:
  TState IntegrateStep(const TSystem& system,
                   const TState& state,
                   float start_time,
                   float step) const override {
    auto der = system.ComputeTimeDerivative(state, start_time);
    return state + der * step;
  }
};
}  // namespace GLOO