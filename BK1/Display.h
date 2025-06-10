#ifndef DISPLAY_H
#define DISPLAY_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>

class Display {
private:
    sf::Font font;
    sf::Text livesText, scoreText, timeText;

public:
    int lives = 1;
    int score = 0;
    float remainingTime = 90.f; // giây

    Display();                                 // Constructor
    void update(int newLives, int newScore, sf::Time timeLeft);
    // Cập nhật văn bản hiển thị
    void draw(sf::RenderWindow& window);       // Vẽ lên cửa sổ
    void reset(int newLives = 1, int newScore = 0, float newTime = 90.f); // Reset
};

#endif // DISPLAY_H
