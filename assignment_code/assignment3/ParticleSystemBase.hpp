#pragma once

#include "ParticleState.hpp"

namespace GLOO {
class ParticleSystemBase {
 public:
  virtual ~ParticleSystemBase() {
  }

  virtual ParticleState ComputeTimeDerivative(const ParticleState& state,
                                              float time) const = 0;
};
}  // namespace GLOO
