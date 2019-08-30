#include "Camera/PerspectiveCamera.hpp"
#include "Model.hpp"
#include "Renderer/Renderer.hpp"
#include "SFML/Graphics.hpp"
#include <atomic>
#include <chrono>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, const char **argv) {
  const auto width = 800;
  const auto height = 600;

  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " /path/to/model/file" << endl;
    return 1;
  }

  shared_ptr<Geometry> geometry = Geometry::Box(1, 1, 1);
  // Material mat;
  // auto cube = make_shared<Mesh>(geometry);

  string path(argv[1]);
  Model model(path);
  Renderer renderer;
  Scene scene;
  //  scene.add(cube);
  for_each(model.meshes.begin(), model.meshes.end(),
           [&scene](auto &mesh) { scene.add(mesh); });
  PerspectiveCamera camera(45, width, height, 0, 1000);
  camera.setPosition(1.0f, 1.0f, 3.0f);
  float radian = 0.0f;

  // display
  sf::RenderWindow window(sf::VideoMode(width, height), "Rasterizer");
  sf::Texture texture;
  texture.create(width, height);
  sf::Sprite sprite;
  sprite.setTexture(texture);
  vector<sf::Uint8> pixels(width * height * 4, 255);
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
    window.clear(sf::Color::Black);

    auto start = chrono::steady_clock::now();

    auto frame = renderer.render(scene, camera);

    auto delta = chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start);
    cout << (to_string(delta.count()) + " ms") << endl;

    camera.setPosition(3.0f * sinf(radian), 1.0f, 3.0f * cosf(radian));
    radian += 0.01f;

    for (int i = 0; i < pixels.size(); i++) {
      pixels[i] = uint8_t(frame.colors[i] * 255);
    }
    texture.update(pixels.data());

    window.draw(sprite);
    window.display();
  }

  return 0;
}