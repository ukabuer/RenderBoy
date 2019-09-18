#include "Camera/OrbitControl.hpp"
#include "Camera/PerspectiveCamera.hpp"
#include "Model.hpp"
#include "Light/PointLight.hpp"
#include "Renderer/RasterizationRenderer.hpp"
#include "SFML/Graphics.hpp"
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
  auto renderer = RasterizationRenderer();
  Scene scene{};
  for (auto &mesh : model.meshes) {
    scene.add(mesh);
  }

  auto light = make_shared<PointLight>();
  light->setColor(255, 255, 255, 0);
  light->setPosition(1, 1, 1);
  scene.add(light);

  PerspectiveCamera camera(60, width, height, 0.1f, 100.f);
  camera.setPosition(0.0f, 0.0f, 3.0f);

  auto controller = OrbitController(camera);

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
      const auto scrollDelta = event.mouseWheelScroll.delta;
      const auto mouse = sf::Mouse::getPosition(window);
      const auto x = mouse.x;
      const auto y = mouse.y;
      switch (event.type) {
      case sf::Event::Closed:
        window.close();
        break;
      case sf::Event::MouseWheelScrolled:
        if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
          controller.updateZoom(scrollDelta, -x, -y);
        }
        break;
      case sf::Event::MouseButtonPressed:
        if (event.mouseButton.button == sf::Mouse::Left) {
          controller.updateOrbit(true, -x, -y);
        } else if (event.mouseButton.button == sf::Mouse::Right) {
          controller.updatePan(true, -x, -y);
        }
        break;
      case sf::Event::MouseButtonReleased:
        if (event.mouseButton.button == sf::Mouse::Left) {
          controller.updateOrbit(false, -x, -y);
        } else if (event.mouseButton.button == sf::Mouse::Right) {
          controller.updatePan(false, -x, -y);
        }
        break;
      case sf::Event::MouseMoved:
        controller.update(-x, -y);
        break;
      default:
        break;
      }
    }

    window.clear(sf::Color::Black);

    auto start = chrono::steady_clock::now();

    auto frame = renderer.render(scene, camera);

    auto delta = chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start);
    cout << "\r" << to_string(delta.count()) + " ms";

    for (auto i = 0; i < height; i++) {
      for (auto j = 0; j < width; j++) {
        auto offset1 = j + (height - i - 1) * width;
        auto offset2 = j + i * width;
        offset1 *= 4;
        offset2 *= 4;
        pixels[offset1] = frame.colors[offset2];
        pixels[offset1 + 1] = frame.colors[offset2 + 1];
        pixels[offset1 + 2] = frame.colors[offset2 + 2];
        pixels[offset1 + 3] = 255;
      }
    }
    texture.update(pixels.data());

    window.draw(sprite);
    window.display();
  }

  return 0;
}
