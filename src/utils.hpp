#pragma once
#include "Camera.hpp"
#include "Scene.hpp"
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>

struct RenderConfig {
  Scene scene;
  Camera camera;

  static auto Load(const std::string &filename) -> RenderConfig;
};

template <typename T, typename Fn>
void ParallelForEach(T begin, T end, const Fn &callback) {
  auto threads = std::vector<std::thread>();
  auto max = std::thread::hardware_concurrency();
  std::atomic<T> next(begin);

  for (auto threadId = 0u; threadId < max; threadId++) {
    threads.emplace_back([&callback, &next, end]() {
      while (true) {
        auto idx = next.fetch_add(1);
        if (idx >= end) {
          break;
        }
        callback(idx);
      }
    });
  }

  for (auto &t : threads) {
    t.join();
  }
}
