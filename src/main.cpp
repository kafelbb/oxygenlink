#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <filesystem>

#include "extadb.h"
#include "ui.h"
#include "logger.h"
#include "icongrabber.h"

#include <SFML/Graphics.hpp>
#include <windows.h>

std::vector<std::string> shared_pkgs;
std::vector<sf::Texture> shared_tex;
std::mutex adb_mutex;
std::atomic<bool> adb_data_ready(false);

void adb_worker() {
    std::vector<std::string> local_pkgs = get_installed_pkgs();

    check_cached();
    check_apps(local_pkgs);

    {
        std::lock_guard<std::mutex> lock(adb_mutex);
        shared_pkgs = std::move(local_pkgs);
    }
    adb_data_ready = true;
}

void check_ev(const sf::Event& ev, sf::RenderWindow& win, float& target_scroll ,float& current_scroll, float& max_scroll) {
	if (ev.type == sf::Event::Closed) {
		win.close();
	}
	if (ev.type == sf::Event::MouseWheelScrolled) {
		if (ev.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
			target_scroll += ev.mouseWheelScroll.delta * 50.0f;

			if (target_scroll > 0.0f) target_scroll = 0.0f;
			if (target_scroll < -max_scroll) target_scroll = -max_scroll;
		}
	}
	if (ev.type == sf::Event::MouseButtonReleased) {
		if (ev.mouseButton.button == sf::Mouse::Left) {
			btnup_buttons(ev.mouseButton.x, ev.mouseButton.y - static_cast<int>(current_scroll));
		}
	}
	if (ev.type == sf::Event::MouseButtonPressed) {
		if (ev.mouseButton.button == sf::Mouse::Left) {
			btndown_buttons(ev.mouseButton.x, ev.mouseButton.y - static_cast<int>(current_scroll));
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	std::cout << "oxygenlink " << PROG_VER << std::endl;

	sf::ContextSettings st;
	st.antialiasingLevel = 8;

	sf::RenderWindow win(sf::VideoMode(500, 500), "oxygenlink", sf::Style::Close, st);
	//win.setVerticalSyncEnabled(true);
	win.setActive(true);

	sf::Image icon;
	if (icon.loadFromFile("tools/app.png")) {
		win.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	}

	win.clear(sf::Color(0, 0, 0));

	sf::Texture logotex;
	sf::Sprite logospr;
	logotex.loadFromFile("tools/link.png");
	logotex.setSmooth(true);
	logospr.setTexture(logotex);
	logospr.setPosition(29.0f, 454.0f);

	sf::Texture gradtex;
	sf::Sprite gradspr;
	gradtex.loadFromFile("tools/grad.png");
	gradtex.setSmooth(true);
	gradspr.setTexture(gradtex);
	gradspr.setPosition(0.0f, 0.0f);

	sf::Texture holdtex;
	sf::Sprite holdspr;
	holdtex.loadFromFile("tools/holdon.png");
	holdtex.setSmooth(true);
	holdspr.setTexture(holdtex);
	holdspr.setPosition(0.0f, 0.0f);

	sf::Texture loadtex;
	sf::Sprite loadspr;
	loadtex.loadFromFile("tools/loader.png");
	loadtex.setSmooth(true);
	loadspr.setTexture(loadtex);
	sf::Vector2u load_size = loadtex.getSize();
	loadspr.setOrigin(load_size.x / 2.0f, load_size.y / 2.0f);
	loadspr.setPosition(459.0f, 461.0f);

	std::vector<std::string> pkgs;

	bool debounce = false;
	float loader_alpha = 255.0f;
	float fade_speed = 300.0f;

	float target_scroll = 0.0f;
	float current_scroll = 1000.0f;
	float max_scroll = 0.0f;

	std::thread adb_thread(adb_worker);
	adb_thread.detach();

	sf::Clock clock;

	bool needs_update = true;

	int mouse_x = 0;
	int mouse_y = 0;

	while (win.isOpen()) {
		sf::Event ev;
		if (needs_update) {
			while (win.pollEvent(ev)) {
				if (ev.type == sf::Event::MouseMoved) {
					mouse_x = ev.mouseMove.x;
					mouse_y = ev.mouseMove.y;
				}
				else {
					check_ev(ev, win, target_scroll, current_scroll, max_scroll);
				}
			}
		}
		else {
			if (win.waitEvent(ev)) {
				if (ev.type == sf::Event::MouseMoved) {
					mouse_x = ev.mouseMove.x;
					mouse_y = ev.mouseMove.y;
				}
				else {
					check_ev(ev, win, target_scroll, current_scroll, max_scroll);
				}
				needs_update = true;
			}
		}

		float dt = clock.restart().asSeconds();

		if (adb_data_ready && !debounce) {
			std::lock_guard<std::mutex> lock(adb_mutex);
			debounce = true;

			int start_x = 35, start_y = 60, btn_size = 64, padding = 25;
			int max_y = start_y;

			shared_tex.resize(shared_pkgs.size());

			for (size_t i = 0; i < shared_pkgs.size(); ++i) {
				int row = i / 5;
				int col = i % 5;
				int x = start_x + col * (btn_size + padding);
				int y = start_y + row * (btn_size + padding);

				if (shared_tex[i].loadFromFile("cached/" + shared_pkgs[i] + ".png")) {
					shared_tex[i].setSmooth(true);
				}

				button::create(x, y, btn_size, btn_size, shared_pkgs[i], shared_tex[i]);

				if (y > max_y) max_y = y;
			}

			max_scroll = static_cast<float>(max_y + btn_size + 80) - 500.0f;
			if (max_scroll < 0.0f) max_scroll = 0.0f;

			needs_update = true;
		}

		bool is_animating = false;

		if (debounce) {
			float scroll_diff = target_scroll - current_scroll;
			if (std::abs(scroll_diff) > 0.1f) {
				current_scroll += scroll_diff * 12.0f * dt;
				is_animating = true;
			}
			else {
				current_scroll = target_scroll;
			}
		}

		hover_buttons(mouse_x, mouse_y - static_cast<int>(current_scroll), win);

		if (loader_alpha > 0.0f) {
			loadspr.rotate(250.0f * dt);
			is_animating = true;
			if (debounce) {
				loader_alpha -= fade_speed * dt;
				if (loader_alpha < 0.0f) {
					loader_alpha = 0.0f;
				}
				loadspr.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(loader_alpha)));
				holdspr.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(loader_alpha)));
			}
		}
		
		win.clear(sf::Color(1, 1, 1));

		draw_ui(win, current_scroll);
		win.draw(gradspr);
		win.draw(holdspr);
		win.draw(loadspr);
		win.draw(logospr);

		win.display();

		needs_update = is_animating || !debounce;

		if (needs_update) {
			sf::sleep(sf::milliseconds(5));
		}
	}

	//exec_com("tools\\scrcpy\\adb kill-server");

	return 0;
}