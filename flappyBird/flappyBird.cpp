#include <iostream>
#include <SFML\Graphics.hpp>

unsigned int screen_width = 800;
unsigned int screen_height = 600;

int main() {
	sf::RenderWindow window(sf::VideoMode({ screen_width, screen_height }), "Flappy Bird", sf::Style::Titlebar | sf::Style::Close);
	window.setFramerateLimit(60);

	const float gravity = 0.8f;

	sf::Texture flappyBirdTexture;
	if (!flappyBirdTexture.loadFromFile("flappybirdtxtr.png")) {
		return -1;
	}
	sf::Sprite flappyBirdSprite(flappyBirdTexture);
	flappyBirdSprite.setScale({ 0.2f, 0.2f });

	float flappyBird_x = 100.f;
	float flappyBird_y = 100.f;
	float flappyBird_y_speed = 0.f;
	float flappyRotation = 0.f;
	sf::FloatRect flappySize = flappyBirdSprite.getLocalBounds();
	float f_width = flappySize.size.x;
	float f_height = flappySize.size.y;

	sf::CircleShape testPoint(10.f);
	testPoint.setPosition({ flappyBird_x, flappyBird_y });
	testPoint.setFillColor(sf::Color::Red);

	while (window.isOpen()) {
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				if (keyPressed->code == sf::Keyboard::Key::Space) {
					flappyBird_y_speed = -15.f;
				}
			}
		}
		flappyBird_y_speed += gravity;
		flappyBird_y += flappyBird_y_speed;

		flappyBirdSprite.setPosition({ flappyBird_x, flappyBird_y });
		testPoint.setPosition({ flappyBird_x, flappyBird_y });
		flappyBirdSprite.setOrigin({ f_width / 2.f, f_height / 2.f});
		flappyBirdSprite.setRotation(sf::radians(std::atan(flappyBird_y_speed / 45)));

		window.clear(sf::Color::Black);
		window.draw(flappyBirdSprite);
		//window.draw(testPoint);
		window.display();
	}

	return 0;
}