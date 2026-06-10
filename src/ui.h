#pragma once
#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>

class button {
public:
	button(int x, int y, int w, int h, std::string pkg, const sf::Texture& ready_texture);

	std::string pkg = "app.settings";

	int x;
	int y;
	int w;
	int h;

	bool hovered = false;
	bool pressed = false;

	int check(int mx, int my);
	void draw(sf::RenderWindow& window, float offset);
	static void create(int x, int y, int w, int h, std::string pkg, const sf::Texture& ready_texture);

	sf::Texture texture;
	sf::Sprite sprite;
};

extern std::vector<std::unique_ptr<button>> btns;

void btnup_buttons(int mx, int my);
void btndown_buttons(int mx, int my);
void hover_buttons(int mx, int my, sf::RenderWindow& win);
void draw_ui(sf::RenderWindow& window, float scroll_offset);