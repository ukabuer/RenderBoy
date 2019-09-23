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

  Camera(uint32_t width, uint32_t height, Type type = Type::Perspective): type(type) {
    frame.width = width;
    frame.height = height;
    frame.size = width * height;
    frame.aspect = static_cast<float>(width) / static_cast<float>(height);
    frame.z.resize(frame.size);
    frame.colors.resize(frame.size * 4);
  }

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

  auto getWidth() const -> uint32_t { return this->frame.width; }
  void setWidth(uint32_t w);

  auto getHeight() const -> uint32_t { return this->frame.height; }
  void setHeight(uint32_t h);

  void setZ(size_t idx, float z) { frame.z[idx] = z; }

  void setZ(uint32_t x, uint32_t y, float z) {
    const auto idx = static_cast<size_t>(x + y * frame.width);
    frame.z[idx] = z;
  }

  void setColor(size_t idx, const Eigen::Vector3f &color) {
    idx = idx << 2;
    frame.colors[idx] = color[0];
    frame.colors[idx + 1] = color[1];
    frame.colors[idx + 2] = color[2];
    frame.colors[idx + 3] = 1.0f;
  }

  void setColor(uint32_t x, uint32_t y, const Eigen::Vector3f &color) {
    const auto idx = static_cast<size_t>(x + y * frame.width);
    this->setColor(idx, color);
  }

  auto getFrame() const -> const Frame & { return frame; }

  void clearFrame(const Eigen::Vector3f &color = {0.0f, 0.0f, 0.0f},
                  float z = -FLT_MAX);

private:
  Type type;

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

  Frame frame = {};
};
