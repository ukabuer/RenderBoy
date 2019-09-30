#pragma once
#include "Frame.hpp"
#include <Eigen/Core>
#include <cstdint>

class Camera {
public:
  enum class Type : uint32_t {
    Perspective,
    Orthographic,
  };

  Camera() = delete;

  Camera(uint32_t width, uint32_t height, Type type = Type::Perspective)
      : type(type),
        aspect(static_cast<float>(width) / static_cast<float>(height)),
        frame(width, height) {}

  void lookAt(const Eigen::Vector3f &position, const Eigen::Vector3f &target,
              const Eigen::Vector3f &up) noexcept;

  void setProjection(float left, float right, float top, float bottom,
                     float near, float far) noexcept;

  void setProjection(float fov, float near, float far) noexcept;

  auto getPosition() const -> const Eigen::Vector3f & { return position; }

  auto getUp() -> const Eigen::Vector3f & { return up; }

  auto getTarget() -> const Eigen::Vector3f & { return target; }

  void setPosition(float x, float y, float z);

  void setPosition(const Eigen::Vector3f &position);

  void setTarget(float x, float y, float z);
  void setTarget(const Eigen::Vector3f &target);

  void setUp(float x, float y, float z);
  void setUp(const Eigen::Vector3f &up);

  auto getViewMatrix() const noexcept -> const Eigen::Matrix4f & {
    return view;
  }

  auto getProjectionMatrix() const noexcept -> const Eigen::Matrix4f & {
    return projection;
  }

  auto getNear() const noexcept -> float;

  auto getFar() const noexcept -> float;

  auto getWidth() const -> uint32_t { return this->frame.getWidth(); }

  void setWidth(uint32_t w);

  auto getHeight() const -> uint32_t { return this->frame.getHeight(); }

  void setHeight(uint32_t h);

  auto getFrame() -> Frame & { return frame; }

private:
  Type type;

  float aspect = 0.f;

  Eigen::Vector3f position = {0.f, 0.f, 0.f};
  Eigen::Vector3f target = {0.f, 0.f, 0.f};
  Eigen::Vector3f up = {0.f, 1.f, 0.f};

  float left = 0.f;
  float right = 0.f;
  float top = 0.f;
  float bottom = 0.f;
  float near = 0.01f;
  float far = 1000.f;

  Eigen::Matrix4f view = Eigen::Matrix4f::Zero();
  Eigen::Matrix4f projection = Eigen::Matrix4f::Zero();

  Frame frame;
};
