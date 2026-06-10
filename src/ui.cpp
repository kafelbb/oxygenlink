#pragma once
#include "ui.h"
#include "logger.h"
#include "extadb.h"

#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>

void btn_con(std::string pkg) {
    launch_app(pkg);
}

button::button(int x, int y, int w, int h, std::string pkg, const sf::Texture& ready_texture)
    : x(x), y(y), w(w), h(h), pkg(pkg), texture(ready_texture)
{
    sprite.setTexture(texture);
    sprite.setPosition(static_cast<float>(x), static_cast<float>(y));
    sprite.setColor(sf::Color(215, 215, 215));

    sf::Vector2u texture_size = texture.getSize();
    float scale_x = static_cast<float>(w) / texture_size.x;
    float scale_y = static_cast<float>(h) / texture_size.y;
    sprite.setScale(scale_x, scale_y);
}

void button::create(int x, int y, int w, int h, std::string pkg, const sf::Texture& ready_texture) {
    btns.push_back(std::make_unique<button>(x, y, w, h, pkg, ready_texture));
}

std::vector<std::unique_ptr<button>> btns;

int button::check(int mx, int my) {
    if (mx > x && mx < x + w && my > y && my < y + h) {
        return 1;
    }
    return 0;
}

void btnup_buttons(int mx, int my) {
    for (auto& btn : btns) {
        if (btn->check(mx, my)) {
            btn->pressed = true;
            btn->sprite.setColor(sf::Color(215, 215, 215));
            btn_con(btn->pkg);
            btn->pressed = false;
        }
    }
}

void btndown_buttons(int mx, int my) {
    for (auto& btn : btns) {
        if (btn->check(mx, my)) {
            btn->pressed = true;
            btn->sprite.setColor(sf::Color(150, 150, 150));
        }
    }
}

void hover_buttons(int mx, int my, sf::RenderWindow& win) {
    for (auto& btn : btns) {
        bool currently_hovered = btn->check(mx, my);

        if (currently_hovered != btn->hovered) {
            btn->hovered = currently_hovered;

            if (btn->hovered) {
                btn->sprite.setColor(sf::Color(255, 255, 255));
            }
            else {
                btn->sprite.setColor(sf::Color(215, 215, 215));
            }
        }
    }
}

void button::draw(sf::RenderWindow& window, float offset) {
    float render_y = static_cast<float>(y + offset);

    if (render_y + h < 0 || render_y > 500) {
        return;
    }

    sprite.setPosition(static_cast<float>(x), render_y);
    window.draw(sprite);
}

void draw_ui(sf::RenderWindow& window, float offset) {
    for (auto& btn : btns) {
        btn->draw(window, offset);
    }
}