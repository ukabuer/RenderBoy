#include "Camera/OrbitControl.hpp"
#include "Camera/PerspectiveCamera.hpp"
#include "Light.hpp"
#include "Model.hpp"
#include "Renderer/RasterizationRenderer.hpp"
#include "utils.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, const char **argv) {
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " /path/to/config/file" << endl;
    return 1;
  }

  auto config = RenderConfig::Load(argv[1]);
  shared_ptr<Camera> camera = move(config.camera);
  auto scene = move(config.scene);
  auto renderer = RasterizationRenderer();
  auto controller =
      OrbitController(*dynamic_cast<PerspectiveCamera *>(camera.get()));

  // display
  const auto width = camera->getWidth();
  const auto height = camera->getHeight();
  sf::RenderWindow window(sf::VideoMode(width, height), "RenderBoy");
  sf::Texture texture;
  texture.create(width, height);
  sf::Sprite sprite;
  sprite.setTexture(texture);
  sprite.setOrigin({0, sprite.getLocalBounds().height});
  sprite.setScale(1, -1);
  vector<sf::Uint8> pixels(width * height * 4, 255);
  while (window.isOpen()) {
    auto event = sf::Event();
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

    // render
    renderer.render(scene, *camera);

    auto &frame = camera->getFrame();
    for (auto i = 0; i < frame.colors.size(); i++) {
      pixels[i] = static_cast<unsigned char>(frame.colors[i] * 255.0f);
    }

    auto delta = chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start);
    cout << "\r" << to_string(delta.count()) + " ms";

    texture.update(pixels.data());

    window.draw(sprite);
    window.display();
  }

  return 0;
}
