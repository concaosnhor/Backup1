#include "Display.h"
#include <string>
#include <sstream>
#include <iomanip> // Thêm để dùng std::setw và std::setfill

Display::Display() {
    if (!font.loadFromFile("fonts/Emulator.otf")) {
        // Thông báo lỗi nếu không tải được font
        std::cerr << "Failed to load font: fonts/Emulator.otf" << std::endl;
    }

    // Cài đặt văn bản cho điểm
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::Yellow);
    scoreText.setPosition(10.f, 10.f);

    // Cài đặt văn bản cho mạng
    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::Red);
    livesText.setPosition(10.f, 40.f);

    // Cài đặt văn bản cho thời gian
    timeText.setFont(font);
    timeText.setCharacterSize(24);
    timeText.setFillColor(sf::Color::Blue);
    timeText.setPosition(10.f, 70.f);
}

void Display::update(int score, int lives, sf::Time elapsedTime) {
    scoreText.setString("Score: " + std::to_string(score));
    livesText.setString("Lives: " + std::to_string(lives));

    int totalSeconds = static_cast<int>(elapsedTime.asSeconds());
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;

    std::ostringstream oss;
    oss << "Time: " << std::setw(2) << std::setfill('0') << minutes
        << ":" << std::setw(2) << std::setfill('0') << seconds;

    timeText.setString(oss.str());
}

void Display::draw(sf::RenderWindow& window) {
    window.draw(scoreText);
    window.draw(livesText);
    window.draw(timeText);
}
