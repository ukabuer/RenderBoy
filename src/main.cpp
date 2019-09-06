#include "Camera/PerspectiveCamera.hpp"
#include "Model.hpp"
#include "Renderer/RasterizationRenderer.hpp"
#include "SFML/Graphics.hpp"
#include <atomic>
#include <chrono>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, const char **argv) {
  const auto width = 800u;
  const auto height = 600u;

  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " /path/to/model/file" << endl;
    return 1;
  }

  string path(argv[1]);
  Model model(path);
  auto renderer = make_unique<RasterizationRenderer>();
  Scene scene;
  for_each(model.meshes.begin(), model.meshes.end(),
           [&scene](auto &mesh) { scene.add(mesh); });
  PerspectiveCamera camera(45, width, height, 0, 1000);
  float radian = 0.0f;

  // display
  sf::RenderWindow window(sf::VideoMode(width, height), "RenderBoy");
  sf::Texture texture;
  texture.create(width, height);
  sf::Sprite sprite;
  sprite.setTexture(texture);
  vector<sf::Uint8> pixels(width * height * 4, 255);
  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
    window.clear(sf::Color::Black);

    camera.setPosition(2.0f * sinf(radian), 0.0f, 2.0f * cosf(radian));
    radian += 0.01f;

    auto start = chrono::steady_clock::now();

    auto frame = renderer->render(scene, camera);

    auto delta = chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start);
    cout << "\r" << (to_string(delta.count()) + " ms");

    for (auto i = 0; i < height; i++) {
      for (auto j = 0; j < width; j++) {
        auto offset1 = j + (height - i - 1) * width;
        auto offset2 = j + i * width;
        offset1 *= 4;
        offset2 *= 4;
        pixels[offset1] = uint8_t(frame.colors[offset2]);
        pixels[offset1 + 1] = uint8_t(frame.colors[offset2 + 1]);
        pixels[offset1 + 2] = uint8_t(frame.colors[offset2 + 2]);
        pixels[offset1 + 3] = 255;
      }
    }
    texture.update(pixels.data());

    window.draw(sprite);
    window.display();
  }

  return 0;
}