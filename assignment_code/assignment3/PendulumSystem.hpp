#pragma once

#include "ParticleSystemBase.hpp"

namespace GLOO {
class PendulumSystem : public ParticleSystemBase {
public:
  void AddParticle(float mass, float drag) {
    masses_.push_back(mass);
    drag_coefs_.push_back(drag);
    fixed_.push_back(false);
  }
  void AddSpring(size_t i, size_t j, float coef, const ParticleState& state) {
    springs_.emplace_back(i, j, glm::length(state.positions[i] - state.positions[j]), coef);
  }
  void setFix(size_t i, bool fix) {
    fixed_[i] = fix;
  }
  const std::vector<std::tuple<size_t, size_t, float, float>>& GetSprings() {
    return springs_;
  }
  ParticleState ComputeTimeDerivative(const ParticleState& state,
                                      float time) const override {
    assert(fixed_.size() == state.positions.size());
    const glm::vec3 g{0, 0, -2};
    std::vector<glm::vec3> acc(state.positions.size(), g);
    for (size_t i = 0; i < state.positions.size(); ++i) {
      acc[i] -= drag_coefs_[i] * state.velocities[i] / masses_[i];
    }
    for(auto& [i, j, len, coef] : springs_) {
      auto d = state.positions[i] - state.positions[j];
      auto force = coef * (glm::length(d) - len) * glm::normalize(d);
      acc[i] -= force / masses_[i];
      acc[j] += force / masses_[j];
    }
    auto vels = state.velocities;
    for(size_t i = 0; i < state.positions.size(); ++i) {
      if (fixed_[i])
        acc[i] = vels[i] = {0, 0, 0};
    }
    return ParticleState{.positions = vels, .velocities = acc};
  }
private:
  std::vector<bool>fixed_;
  std::vector<std::tuple<size_t,size_t,float,float>> springs_;
  std::vector<float> masses_;
  std::vector<float> drag_coefs_;
};
}
