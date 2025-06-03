#pragma once

#include <Visualization/Model/Entities/Axes.hpp>
#include <Visualization/Model/Entities/Grid.hpp>
#include <Visualization/Model/Entities/model.h>
#include <Visualization/Model/Entities/shaderComponent.h>
#include <Visualization/Model/Entity.hpp>
#include <Visualization/Model/Shaders/ModelShader.hpp>
#include <Visualization/Model/Shaders/SimpleShader.hpp>

namespace DefaultModelPath {
constexpr char path[] = "data/spaceship.obj";
}

class EntityFactory
{
public:
  static std::shared_ptr<Object3d> createEntity(const std::string &type,
                                                glm::vec3 pos = {0, 0, 0},
                                                float angle = 0) {

    if (type == "Axis") {
      auto shader =
          std::make_shared<Shader>(SimpleShader::simple_vertex_shader,
                                   SimpleShader::simple_fragment_shader);
      return std::make_shared<Axes>(std::move(shader));
    } else if (type == "Grid") {
      auto shader =
          std::make_shared<Shader>(SimpleShader::simple_vertex_shader,
                                   SimpleShader::simple_fragment_shader);
      return std::make_shared<Grid>(std::move(shader));
    } else if (type == "Model") {
      auto shader = std::make_shared<Shader>(
          ModelShader::phong_vertex_shader, ModelShader::phong_fragment_shader);
      return std::make_shared<Model>(DefaultModelPath::path, std::move(shader),
                                     pos, angle);
    }

    return nullptr;
  }
};
