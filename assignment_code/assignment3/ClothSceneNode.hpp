#pragma once

#include "ODESceneNode.hpp"
#include "ForwardEulerIntegrator.hpp"
#include "SimpleSystem.hpp"
#include "PendulumSystem.hpp"
#include "IntegratorType.hpp"

#include "gloo/VertexObject.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/Material.hpp"

#include "external/src/glm-0.9.9.8/glm/gtx/norm.inl"

#include <vector>

namespace GLOO {

constexpr size_t N = 50;
constexpr size_t M = 50;

size_t GetIndex(size_t i, size_t j) {
  return i * M + j;
}

ParticleState GenGrid(size_t n, size_t m, float f) {
  std::vector<glm::vec3>pos;
  for(size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < m; ++j) {
      pos.emplace_back(5 + f * i, f * j, 0);
    }
  }
  return ParticleState(pos);
}


class ClothSceneNode : public ODESceneNode<PendulumSystem, ParticleState> {
private:
  void SphereCollisions() {
    float time_per = cur_time_ - (int) (cur_time_ / 10) * 10;
    const glm::vec3 up{6, 1, 1};
    const glm::vec3 down{6, 1, -3};
    float rad = 1;
    time_per = std::min(time_per, 10 - time_per);
    glm::vec3 pos = (up * time_per + down * (10 - time_per)) / 10.0f;
    sphere_->GetTransform().SetPosition(pos);
    for(auto&x:state_.positions) {
      auto d = x - pos;
      if (glm::length2(d) < rad * rad) {
        x = pos + glm::normalize(d) * rad;
      }
    }
  }
  void onUpdate() override {
    auto pos = make_unique<PositionArray>(state_.positions);
    cloth_mesh_->UpdatePositions(std::move(pos));
    ComputeNormals();
    SphereCollisions();
  }
  SceneNode* cloth_;
  SceneNode* sphere_;
  std::shared_ptr<VertexObject> cloth_mesh_;
  std::shared_ptr<VertexObject> sphere_mesh_;
  std::shared_ptr<PhongShader> shader_;
  void ComputeNormals() {
    auto pos = cloth_mesh_->GetPositions();
    auto normals = make_unique<NormalArray>(pos.size());
    std::vector<int> face_cnt(pos.size());
    auto& ind = cloth_mesh_->GetIndices();
    assert(ind.size() % 3 == 0);
    for (size_t i = 0; i < ind.size(); i += 3) {
      auto normal = glm::cross(pos[ind[i + 1]] - pos[ind[i]], pos[ind[i + 2]] - pos[ind[i]]);
      normal = glm::normalize(normal);
      for(size_t j = i; j < i + 3; ++j) {
        (*normals)[ind[j]] += normal;
        ++face_cnt[ind[j]];
      }
    }
    for(size_t i = 0; i < normals->size(); ++i) {
      (*normals)[i] /= face_cnt[i];
      (*normals)[i] = glm::normalize((*normals)[i]);
    }
    cloth_mesh_->UpdateNormals(std::move(normals));
  };

public:
  ClothSceneNode(IntegratorType type, float integration_step)
      : ODESceneNode(GenGrid(N, M, 3.0f / N), type, integration_step) {
    shader_ = std::make_shared<PhongShader>();
    sphere_mesh_ = PrimitiveFactory::CreateSphere(0.98, 30, 30);
    auto sphere_ptr = make_unique<SceneNode>();
    sphere_ptr->CreateComponent<RenderingComponent>(sphere_mesh_);
    sphere_ptr->CreateComponent<ShadingComponent>(shader_);
    sphere_ = sphere_ptr.get();
    this->AddChild(std::move(sphere_ptr));
    cloth_mesh_ = std::make_shared<VertexObject>();
    for(size_t i = 0; i < N * M; ++i)
      system_.AddParticle(0.5f / N / M, 0.01);
    system_.setFix(GetIndex(0, 0), true);
    system_.setFix(GetIndex(0, M-1), true);
    system_.setFix(GetIndex(N-1, 0), true);
    system_.setFix(GetIndex(N-1, M-1), true);
    for(size_t i = 0; i < N; ++i) {
      for(size_t j = 0; j < M; ++j) {
        if (i + 1 < N)
          system_.AddSpring(GetIndex(i, j), GetIndex(i + 1, j), 1, state_);
        if (j + 1 < M)
          system_.AddSpring(GetIndex(i, j), GetIndex(i, j + 1), 1, state_);
      }
    }
    for(size_t i = 0; i < N; ++i) {
      for(size_t j = 0; j < M; ++j) {
        if (i + 1 < N && j + 1 < M)
          system_.AddSpring(GetIndex(i, j), GetIndex(i + 1, j + 1), 1, state_);
        if (i + 1 < N && j - 1 >= 0)
          system_.AddSpring(GetIndex(i, j), GetIndex(i + 1, j - 1), 1, state_);
      }
    }
    for(size_t i = 0; i < N; ++i) {
      for(size_t j = 0; j < M; ++j) {
        if (i + 2 < N)
          system_.AddSpring(GetIndex(i, j), GetIndex(i + 2, j), 1, state_);
        if (j + 2 < M)
          system_.AddSpring(GetIndex(i, j), GetIndex(i, j + 2), 1, state_);
      }
    }
    auto indices = make_unique<IndexArray>();
    for(size_t i = 0; i + 1 < N; ++i)
      for(size_t j = 0; j + 1 < M; ++j) {
        indices->push_back(GetIndex(i, j));
        indices->push_back(GetIndex(i + 1, j));
        indices->push_back(GetIndex(i + 1, j + 1));
        indices->push_back(GetIndex(i, j));
        indices->push_back(GetIndex(i, j + 1));
        indices->push_back(GetIndex(i + 1, j + 1));
      }
    cloth_mesh_->UpdateIndices(std::move(indices));

    auto ptr = make_unique<SceneNode>();
    ptr->CreateComponent<ShadingComponent>(shader_);
    ptr->CreateComponent<RenderingComponent>(cloth_mesh_);

    glm::vec3 color(1.f, 0.5f, 0.3125f);
    auto material = std::make_shared<Material>(color, color, color, 10.0f);
    ptr->CreateComponent<MaterialComponent>(material);

    cloth_ = ptr.get();
    this->AddChild(std::move(ptr));
  }
};
}

