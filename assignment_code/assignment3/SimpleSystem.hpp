#pragma once

#include "ParticleSystemBase.hpp"

namespace GLOO {
class SimpleSystem : public ParticleSystemBase {
public:
  ParticleState ComputeTimeDerivative(const ParticleState& state,
                                              float time) const override {
    auto pos = state.positions[0];
    return ParticleState{.positions = {{-pos.y, pos.x, pos.z}}, .velocities = {{0,0,0}}};
  }
};
}
