#include <iostream>
#include <vector>
#include <SFML\Graphics.hpp>
#include <random>
#include <cmath>
#include <array>
#include <string>

unsigned int screen_width = 600;
unsigned int screen_height = 800;

struct Pipe {
	float gap_y;
	float position_x;
	bool passed = false;
};

int main() {
	sf::RenderWindow window(sf::VideoMode({ screen_width, screen_height }), "Flappy Bird", sf::Style::Titlebar | sf::Style::Close);
	window.setFramerateLimit(60);

	const float gravity = 0.8f;
	bool Start = false, Lost = false, GameOver = false;
	float bobbingAnimationAngle = 0.f;
	int currentframe = 0;
	int totalframes = 3;
	float groundMovement = 0.f;
	sf::Clock clock;
	int Score = 0;

	sf::Texture flappyBirdTexture1, flappyBirdTexture2, flappyBirdTexture3;	// Flappy Bird sprite animation
	if (!flappyBirdTexture1.loadFromFile("assets/yellowbird-upflap.png")) { return -1; }
	if (!flappyBirdTexture2.loadFromFile("assets/yellowbird-midflap.png")) { return -1; }
	if (!flappyBirdTexture3.loadFromFile("assets/yellowbird-downflap.png")) { return -1; }
	sf::Sprite flappyBirdSprite(flappyBirdTexture1);
	flappyBirdSprite.setScale({ 2.f, 2.f });

	sf::Texture messageTexture;												// Start Message
	if (!messageTexture.loadFromFile("assets/message.png")) { return -1; }
	sf::Sprite messageSprite(messageTexture);
	messageSprite.setPosition({ 116.f, 50.f });
	messageSprite.setScale({ 2.f, 2.f });

	sf::Texture gameOverTexture;
	if (!gameOverTexture.loadFromFile("assets/gameover.png")) { return -1; }
	sf::Sprite gameOverSprite(gameOverTexture);
	gameOverSprite.setPosition({ 108.f, 50.f });
	gameOverSprite.setScale({ 2.f, 2.f });

	sf::Texture backgroundTexture, floorTexture;							//Background and Ground sprite/texture loading bs
	if (!backgroundTexture.loadFromFile("assets/background.png")) { return -1; }
	sf::Sprite backgroundSprite(backgroundTexture);
	backgroundSprite.setScale({ 2.1f, 2.1f });
	backgroundSprite.setPosition({ 0.f, -250.f });
	if (!floorTexture.loadFromFile("assets/floor.png")) { return -1; }
	sf::Sprite floorSprite(floorTexture);
	floorSprite.setScale({2.1f, 2.1f});
	floorSprite.setPosition({ 0.f, 700.f });

	std::array<sf::Texture, 10> digitTextures;
	for (int i = 0; i < 10; i++) {
		if (!digitTextures[i].loadFromFile("assets/" + std::to_string(i) + ".png")) { return -1; }
	}
	sf::Sprite digitSprite(digitTextures[0]);

	float flappyBird_x = 100.f;									// Bird specs
	float flappyBird_y = 375.f;
	float flappyBird_y_speed = 0.f;
	float flappyRotation = 0.f;
	sf::FloatRect flappySize = flappyBirdSprite.getLocalBounds();
	float f_width = flappySize.size.x;
	float f_height = flappySize.size.y;

	sf::Texture pipeTexture;									// Pipe specs
	if (!pipeTexture.loadFromFile("assets/pipe.png")) { return -1; }
	sf::Sprite pipeSprite(pipeTexture);
	const float pipeScale = 2.f;
	const float pipeWidth = pipeTexture.getSize().x * pipeScale;

	std::vector<Pipe> pipes;									// Also Pipe specs but with list
	const float pipeGap = 180.f;
	const float pipeSpeed = 3.f;
	const float pipeSpawnTime = 2.f;
	sf::Clock pipeClock;

	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> gapDist(200.f, 500.f);

	const float digitScale = 2.f;

	auto drawNumber = [&](int value, float centerX, float y) {
		std::string s = std::to_string(value);

		float totalWidth = 0.f;
		for (char c : s) {
			totalWidth += digitTextures[c - '0'].getSize().x * digitScale;
		}

		float x = centerX - totalWidth / 2.f;
		for (char c : s) {
			const sf::Texture& tex = digitTextures[c - '0'];
			digitSprite.setTexture(tex, true);
			digitSprite.setScale({ digitScale, digitScale });
			digitSprite.setPosition({ x, y });
			window.draw(digitSprite);
			x += tex.getSize().x * digitScale;
		}
	};

	while (window.isOpen()) {									/// MAIN LOOP ///
		while (const std::optional event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				window.close();
			}
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				if (keyPressed->code == sf::Keyboard::Key::Space) {
					if (GameOver) {
						GameOver = false;
						Score = 0;
						pipes.clear();
						flappyBird_y_speed = 0;
					}
					else {
						if (!Start) { pipeClock.restart(); }
						flappyBird_y_speed = -10.f;
						Start = true;
					}
				}
			}
		}

		if (clock.getElapsedTime().asSeconds() > 0.1f) {
			currentframe = (currentframe + 1) % totalframes;
			if (currentframe == 0) { flappyBirdSprite.setTexture(flappyBirdTexture1); }
			else if (currentframe == 1) { flappyBirdSprite.setTexture(flappyBirdTexture2); }
			else if (currentframe == 2) { flappyBirdSprite.setTexture(flappyBirdTexture3); }
			clock.restart();
		}

		if (!GameOver) {
			if (Start) {															// What happens after the first click a.k.a. the start
				flappyBird_y_speed += gravity;
				flappyBird_y += flappyBird_y_speed;

				if (pipeClock.getElapsedTime().asSeconds() > pipeSpawnTime) {
					pipes.push_back({ gapDist(rng), (float)screen_width });
					pipeClock.restart();
				}

				for (Pipe& p : pipes) {
					p.position_x -= pipeSpeed;
					if (p.position_x < flappyBird_x && !p.passed) {
						Score += 1;
						p.passed = true;
					}
				}

				std::erase_if(pipes, [&](const Pipe& p) {
					return p.position_x + pipeWidth < 0.f;
					});

			}
			else {
				bobbingAnimationAngle += 0.05f;
				flappyBird_y = 35 * std::sin(bobbingAnimationAngle) + 375;
			}

			sf::FloatRect birdBox = flappyBirdSprite.getGlobalBounds();
			for (const Pipe& p : pipes) {
				sf::FloatRect topBox({ p.position_x, 0.f }, { pipeWidth, p.gap_y - pipeGap / 2.f });
				sf::FloatRect bottomBox({ p.position_x, p.gap_y + pipeGap / 2.f }, { pipeWidth, (float)screen_height });
				if (birdBox.findIntersection(topBox) || birdBox.findIntersection(bottomBox)) {
					Lost = true;
				}
			}
			if (flappyBird_y > 680.f || flappyBird_y < 20.f) {
				Lost = true;
			}

			if (Lost) {
				Start = false;
				GameOver = true;
				flappyBird_y_speed = 0;
				Lost = false;
			}
		}

		flappyBirdSprite.setPosition({ flappyBird_x, flappyBird_y });
		flappyBirdSprite.setOrigin({ f_width / 2.f, f_height / 2.f });
		flappyBirdSprite.setRotation(sf::degrees(std::clamp(flappyBird_y_speed * 3.f, -30.f, 90.f)));

		window.clear(sf::Color::Black);
				// Drawing order Back to Front ↓
		window.draw(backgroundSprite);
		for (const Pipe& p : pipes) {
			pipeSprite.setScale({ pipeScale, pipeScale });
			pipeSprite.setPosition({ p.position_x, p.gap_y + pipeGap / 2.f });
			window.draw(pipeSprite);

			pipeSprite.setScale({ pipeScale, -pipeScale });
			pipeSprite.setPosition({ p.position_x, p.gap_y - pipeGap / 2.f });
			window.draw(pipeSprite);
		}
		if (!Start && !GameOver) { window.draw(messageSprite); }

		if (Start && !GameOver) {
			groundMovement -= pipeSpeed;
			if (groundMovement <= -705.6f) { groundMovement = 0; }
		}
		floorSprite.setPosition({ groundMovement, 700.f });
		window.draw(floorSprite);
		floorSprite.setPosition({ groundMovement + 705.6f, 700.f });
		window.draw(floorSprite);
		window.draw(flappyBirdSprite);
		
		if (Start) { drawNumber(Score, screen_width / 2.f, 40.f); }
		if (GameOver) {
			window.draw(gameOverSprite);
			drawNumber(Score, screen_width / 2.f, 350.f);
		}

		// End of drawing
		window.display();
	}

	return 0;
}