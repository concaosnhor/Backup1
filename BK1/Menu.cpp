    // Menu.cpp

    #include "Menu.h"
    #include <iostream>

    Menu::Menu(float width, float height) : selectedItemIndex(0), numberOfItems(MAX_NUMBER_OF_ITEMS) {
        // Load font
        if (!font.loadFromFile("fonts/Emulator.otf")) {
            std::cerr << "Failed to load font\n";
        }

        // Load background texture nếu có
        if (backgroundTexture.loadFromFile("images/BACKGROUNDFINAL.png")) {
            background.setSize(sf::Vector2f(width, height));
            background.setTexture(&backgroundTexture);
            useTexture = true;
        }
        else {
            background.setSize(sf::Vector2f(width, height));
            background.setFillColor(sf::Color(30, 30, 30)); // Nền xám đậm
            useTexture = false;
        }

        // Khởi tạo menu mặc định
        for (int i = 0; i < MAX_NUMBER_OF_ITEMS; i++) {
            menu[i].setFont(font);
            menu[i].setFillColor(i == 0 ? sf::Color::Red : sf::Color::White);
            menu[i].setCharacterSize(40);
            menu[i].setString("Menu Item " + std::to_string(i + 1));
            menu[i].setPosition(width / 2.f - menu[i].getLocalBounds().width / 2, 150 + i * 60);
        }
    }

    Menu::~Menu() {}

    void Menu::draw(sf::RenderWindow& window) {
        if (useTexture || background.getFillColor() != sf::Color::Transparent) {
            window.draw(background);
        }

        for (int i = 0; i < numberOfItems; i++) {
            window.draw(menu[i]);
        }
    }

    void Menu::MoveUp() {
        if (selectedItemIndex - 1 >= 0) {
            menu[selectedItemIndex].setFillColor(sf::Color::White);
            selectedItemIndex--;
            menu[selectedItemIndex].setFillColor(sf::Color::Red);
        }
    }

    void Menu::MoveDown() {
        if (selectedItemIndex + 1 < numberOfItems) {
            menu[selectedItemIndex].setFillColor(sf::Color::White);
            selectedItemIndex++;
            menu[selectedItemIndex].setFillColor(sf::Color::Red);
        }
    }

    void Menu::setNumberOfItems(int num) {
        if (num > 0 && num <= MAX_NUMBER_OF_ITEMS) {
            numberOfItems = num;
            if (selectedItemIndex >= numberOfItems) {
                selectedItemIndex = 0;
            }
        }
    }

    void Menu::setMenuItemString(int index, const std::string& str) {
        if (index >= 0 && index < numberOfItems) {
            menu[index].setString(str);
            float x = menu[index].getPosition().x;
            menu[index].setPosition(x, menu[index].getPosition().y); // Đảm bảo không lệch
        }
    }

    void Menu::updateMenuPositions(float width) {
        for (int i = 0; i < numberOfItems; i++) {
            float xPos = width / 2.f - menu[i].getLocalBounds().width / 2;
            float yPos = 150 + i * 60;
            menu[i].setPosition(xPos, yPos);
        }
    }

    void Menu::updateGameOverMenuPositions(float width) {
        for (int i = 0; i < numberOfItems; i++) {
            float xPos = width / 2.f - menu[i].getLocalBounds().width / 2;
            float yPos = 300 + i * 80;
            menu[i].setPosition(xPos, yPos);
        }
    }
     

    int Menu::getSelectedIndex() const {
        return selectedItemIndex;
    }
