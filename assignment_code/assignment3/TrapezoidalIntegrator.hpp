#pragma once

#include "IntegratorBase.hpp"

namespace GLOO {
template <class TSystem, class TState>
class TrapezoidalIntegrator : public IntegratorBase<TSystem, TState> {
public:
  using IntegratorBase<TSystem, TState>::IntegratorBase;

protected:
  TState IntegrateStep(const TSystem& system,
                       const TState& state,
                       float start_time,
                       float step) const override {
    auto f0 = system.ComputeTimeDerivative(state, start_time);
    auto f1 = system.ComputeTimeDerivative(state + f0 * step, start_time + step);
    return state + (step / 2) * (f0 + f1);
  }
};
}  // namespace GLOO