#include "Camera.hpp"
#include "CameraControls/OrbitControl.hpp"
#include "Renderer/RasterizationRenderer.hpp"
#include "utils.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <vector>

using namespace std;

auto main(int argc, const char **argv) -> int {
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " /path/to/config/file" << endl;
    return 1;
  }

  auto config = RenderConfig::Load(argv[1]);
  auto renderer = RasterizationRenderer();
  auto controller = OrbitController(config.camera);

  // display
  const auto width = config.camera.getWidth();
  const auto height = config.camera.getHeight();
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
          controller.onMouseWheelScroll(scrollDelta);
        }
        break;
      case sf::Event::MouseButtonPressed:
        controller.onMouseDown(event.mouseButton.button == sf::Mouse::Left, x,
                               y);
        break;
      case sf::Event::MouseButtonReleased:
        controller.onMouseUp(event.mouseButton.button == sf::Mouse::Left, x, y);
        break;
      case sf::Event::MouseMoved:
        controller.onMouseMove(x, y);
        break;
      default:
        break;
      }
    }

    window.clear(sf::Color::Black);

    auto start = chrono::steady_clock::now();

    // render
    renderer.render(config.scene, config.camera);

    auto &colors = config.camera.getFrame().getColors();
    for (auto i = 0; i < colors.size(); i++) {
      pixels[i] = static_cast<unsigned char>(colors[i] * 255.0f);
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
