#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>
#include "Menu.h"
#include <iostream>
#include <cmath>
#include "Display.h"
#include "Audio.h"
#include "HighScores.h"

using namespace sf;

Display display;
sf::Clock gameClock;

const int W = 1200;
const int H = 800;

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER,
    HIGH_SCORES,
};

class Animation {
public:
    float Frame, speed;
    Sprite sprite;
    std::vector<IntRect> frames;

    Animation() {}

    Animation(Texture& t, int x, int y, int w, int h, int count, float Speed) {
        Frame = 0;
        speed = Speed;

        for (int i = 0; i < count; i++)
            frames.push_back(IntRect(x + i * w, y, w, h));

        sprite.setTexture(t);
        sprite.setOrigin(w / 2.f, h / 2.f);
        sprite.setTextureRect(frames[0]);
    }

    void update() {
        Frame += speed;
        int n = (int)frames.size();
        if (Frame >= n) Frame -= n;
        sprite.setTextureRect(frames[(int)Frame]);
    }

    bool isEnd() {
        return Frame + speed >= frames.size();
    }
};

class Entity {
public:
    float x, y, dx, dy, R, angle;
    bool life;
    std::string name;
    Animation anim;

    Entity() {
        life = true;
        name = "";
        dx = dy = 0.f;
    }

    void settings(Animation& a, float X, float Y, float Angle = 0.f, float radius = 1.f) {
        anim = a;
        x = X; y = Y;
        angle = Angle;
        R = radius;
    }

    virtual void update() {
        x += dx;
        y += dy;

        if (x > W) x = 0;
        if (x < 0) x = W;
        if (y > H) y = 0;
        if (y < 0) y = H;
    }

    virtual void draw(RenderWindow& app) {
        anim.sprite.setPosition(x, y);
        anim.sprite.setRotation(angle + 90.f);
        app.draw(anim.sprite);
    }
};

class asteroid : public Entity {
public:
    asteroid() {
        name = "asteroid";
    }
    void update() override {
        angle += 1.5f;
        Entity::update();
    }
};

class bullet : public Entity {
public:
    int lifeTime;

    bullet() {
        name = "bullet";
        lifeTime = 60;
    }

    void update() override {
        dx = cos(angle * 3.14159f / 180.f) * 10.f;
        dy = sin(angle * 3.14159f / 180.f) * 10.f;
        x += dx;
        y += dy;

        if (x < 0 || x > W || y < 0 || y > H)
            life = false;

        lifeTime--;
        if (lifeTime <= 0) life = false;
    }

    void settings(Animation& a, float X, float Y, float Angle, float radius) {
        Entity::settings(a, X, Y, Angle, radius);
        dx = cos(angle * 3.14159f / 180.f) * 15.f;
        dy = sin(angle * 3.14159f / 180.f) * 15.f;
        life = true;
        lifeTime = 60;
    }
};

class player : public Entity {
public:
    bool thrust;
    Animation anim_go;

    player() {
        name = "player";
        thrust = false;
    }

    void update() override {
        if (Keyboard::isKeyPressed(Keyboard::Up)) {
            dx += cos(angle * 3.14159f / 180.f) * 0.1f;
            dy += sin(angle * 3.14159f / 180.f) * 0.1f;
        }
        if (Keyboard::isKeyPressed(Keyboard::Down)) {
            dx -= cos(angle * 3.14159f / 180.f) * 0.1f;
            dy -= sin(angle * 3.14159f / 180.f) * 0.1f;
        }

        dx *= 0.98f;
        dy *= 0.98f;

        float maxSpeed = 7.f;
        float speed = sqrt(dx * dx + dy * dy);
        if (speed > maxSpeed) {
            dx = dx / speed * maxSpeed;
            dy = dy / speed * maxSpeed;
        }

        Entity::update();
    }

    void draw(RenderWindow& app) override {
        anim.sprite.setPosition(x, y);
        anim.sprite.setRotation(angle + 90.f);
        app.draw(anim.sprite);
    }
};

bool isCollide(Entity* a, Entity* b) {
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dist = sqrt(dx * dx + dy * dy);
    return dist < (a->R + b->R);
}

int main() {

    bool musicPlaying = false;
    Audio audio;
    audio.playMusic("audio/background.wav");
    audio.loadSound("shoot", "audio/laser.wav");
    audio.loadSound("explosion", "audio/punch-a-rock-161647.wav");


    srand(static_cast<unsigned int>(time(0)));

    RenderWindow app(VideoMode(W, H), "Asteroids with Menu");
    app.setFramerateLimit(60);

    Menu menu(W, H);
    Menu gameOverMenu(W, H);
    Menu pauseMenu(W, H);

    // Cấu hình menu chính
    menu.setNumberOfItems(4);
    menu.setMenuItemString(0, "Play");
    menu.setMenuItemString(1, "High Scores");
    menu.setMenuItemString(2, "About");
    menu.setMenuItemString(3, "Exit");
    menu.updateMenuPositions(W);

    // Cấu hình menu Game Over
    gameOverMenu.setNumberOfItems(3);
    gameOverMenu.setMenuItemString(0, "Restart");
    gameOverMenu.setMenuItemString(1, "Return menu");
    gameOverMenu.setMenuItemString(2, "Exit");
    gameOverMenu.updateGameOverMenuPositions(W);
    bool printedFinalScore = false;  // Khai báo ở ngoài vòng lặp


    // Cấu hình menu Pause
    pauseMenu.setNumberOfItems(2);
    pauseMenu.setMenuItemString(0, "Resume");
    pauseMenu.setMenuItemString(1, "Exit to Menu");
    pauseMenu.updateGameOverMenuPositions(W);


    // Khởi tạo hiển thị điểm số
    sf::Font font;
    if (!font.loadFromFile("fonts/Emulator.otf")) {
        std::cerr << "Error loading font\n";
        return -1;
    }

    GameState currentState = GameState::MENU;
    bool scoreSaved = false;


    // Load textures
    Texture t1, t2, t3, t4, t5, t6, t7;
    if (!t1.loadFromFile("images/spaceship.png") ||
        !t2.loadFromFile("images/background.jpg") ||
        !t3.loadFromFile("images/explosions/type_C.png") ||
        !t4.loadFromFile("images/rock.png") ||
        !t5.loadFromFile("images/fire_blue.png") ||
        !t6.loadFromFile("images/rock_small.png") ||
        !t7.loadFromFile("images/explosions/type_B.png")) {
        std::cerr << "Error loading textures" << std::endl;
        return -1;
    }

    t1.setSmooth(true);
    t2.setSmooth(true);
    Sprite background(t2);
    background.setScale(float(W) / t2.getSize().x, float(H) / t2.getSize().y);

    Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5f);
    Animation sRock(t4, 0, 0, 64, 64, 16, 0.2f);
    Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2f);
    Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8f);
    Animation sPlayer(t1, 40, 0, 40, 40, 1, 0.f);
    Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0.f);
    Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5f);

    std::list<Entity*> entities;
    player* p = nullptr;
    const int MIN_ASTEROIDS = 15;

    auto resetGame = [&]() {
        for (auto e : entities) delete e;
        entities.clear();

        for (int i = 0; i < MIN_ASTEROIDS; i++) {
            asteroid* a = new asteroid();
            a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25.f);
            float speed = ((rand() % 3 + 1) * 0.5f) * 3.f;
            a->dx = cos(a->angle * 3.14159f / 180.f) * speed;
            a->dy = sin(a->angle * 3.14159f / 180.f) * speed;
            entities.push_back(a);
        }

        p = new player();
        p->settings(sPlayer, W / 2.f, H / 2.f, 0.f, 20.f);
        entities.push_back(p);

        // Cập nhật lại trạng thái hiển thị
        display.lives = 3;
        display.score = 0;
        display.remainingTime = 90.f;
        gameClock.restart();
        };
    ;

    resetGame();

    while (app.isOpen()) {
        Event event;
        while (app.pollEvent(event)) {
            if (event.type == Event::Closed) {
                app.close();
            }

            if (currentState == GameState::MENU) {
                printedFinalScore = false;
                if (!musicPlaying) {
                    audio.playMusic("audio/background.wav", true);  // Phát nhạc 1 lần khi vào MENU
                    musicPlaying = true;
                }

                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Up)
                        menu.MoveUp();
                    else if (event.key.code == Keyboard::Down)
                        menu.MoveDown();
                    else if (event.key.code == Keyboard::Enter) {
                        int selected = menu.getSelectedIndex();
                        if (selected == 0) {
                            audio.stopMusic();
                            musicPlaying = false;
                            resetGame();
                            currentState = GameState::PLAYING;

                        }

                        else if (selected == 1) {
                            HighScore hs("highscores.txt");
                            hs.loadScores();  // <--- Bước quan trọng!
                            currentState = GameState::HIGH_SCORES;
                        }


                        else if (selected == 3) {
                            app.close();
                        }
                    }
                }
            }

            else if (currentState == GameState::PLAYING) {
                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Space) {
                        bullet* b = new bullet();
                        b->settings(sBullet, p->x, p->y, p->angle, 10.f);
                        entities.push_back(b);
                        audio.playSound("shoot");

                    }
                    else if (event.key.code == Keyboard::Escape) {
                        currentState = GameState::PAUSED;
                    }
                }
            }
            else if (currentState == GameState::GAME_OVER) {
                if (!scoreSaved) {
                    HighScore hs("highscores.txt");
                    hs.addScore("Top", display.score);
                    hs.saveScores();// hoặc nhận tên từ người chơi                    
                    scoreSaved = true;
                }
                else if (!printedFinalScore) {
                    std::cout << "Final Score: " << display.score << std::endl;
                    printedFinalScore = true;
                }

                else if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Up)
                        gameOverMenu.MoveUp();
                    else if (event.key.code == Keyboard::Down)
                        gameOverMenu.MoveDown();
                    else if (event.key.code == Keyboard::Enter) {
                        int selected = gameOverMenu.getSelectedIndex();
                        if (selected == 0) {
                            audio.stopMusic();
                            musicPlaying = false;

                            resetGame();
                            scoreSaved = false;
                            currentState = GameState::PLAYING;
                        }
                        else if (selected == 1) {
                            resetGame();
                            currentState = GameState::MENU;
                        }
                        else if (selected == 2) {
                            app.close();
                        }
                    }
                }
            }


            else if (currentState == GameState::PAUSED) {
                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Up)
                        pauseMenu.MoveUp();
                    else if (event.key.code == Keyboard::Down)
                        pauseMenu.MoveDown();
                    else if (event.key.code == Keyboard::Enter) {
                        int selected = pauseMenu.getSelectedIndex();
                        if (selected == 0) {
                            currentState = GameState::PLAYING;
                        }
                        else if (selected == 1) {
                            currentState = GameState::MENU;
                            musicPlaying = false;
                        }
                    }
                }
            }
            else if (currentState == GameState::HIGH_SCORES) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        currentState = GameState::MENU;
                    }
                }
            }

        }

        if (currentState == GameState::PLAYING) {
            sf::Time elapsed = gameClock.getElapsedTime();
            display.update(display.score, display.lives, elapsed);

            // ===== LOGIC GAME =====
            if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle += 3.f;
            if (Keyboard::isKeyPressed(Keyboard::Left)) p->angle -= 3.f;
            p->thrust = Keyboard::isKeyPressed(Keyboard::Up);

            for (auto a : entities)
                for (auto b : entities) {
                    if (a->name == "asteroid" && b->name == "bullet" && isCollide(a, b)) {
                        a->life = b->life = false;
                        Entity* e = new Entity();
                        e->settings(sExplosion, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e);
                        audio.playSound("explosion");
                        display.score += 10;

                        for (int i = 0; i < 2; i++) {
                            if (a->R == 15.f) continue;
                            Entity* small = new asteroid();
                            small->settings(sRock_small, a->x, a->y, rand() % 360, 15.f);
                            float speed = ((rand() % 3 + 1) * 0.5f) * 3.f;
                            small->dx = cos(small->angle * 3.14159f / 180.f) * speed;
                            small->dy = sin(small->angle * 3.14159f / 180.f) * speed;
                            entities.push_back(small);
                        }
                    }
                    else if (a->name == "player" && b->name == "asteroid" && isCollide(a, b)) {
                        b->life = false;

                        Entity* e = new Entity();
                        e->settings(sExplosion_ship, a->x, a->y);
                        e->name = "explosion";
                        entities.push_back(e);

                        display.lives--;

                        if (display.lives <= 0) {
                            currentState = GameState::GAME_OVER;
                        }
                        else {
                            // Đặt lại vị trí tàu
                            p->x = W / 2.f;
                            p->y = H / 2.f;
                            p->dx = p->dy = 0.f;
                        }
                    }

                }

            p->anim = p->thrust ? sPlayer_go : sPlayer;

            for (auto e : entities)
                if (e->name == "explosion" && e->anim.isEnd())
                    e->life = false;

            int countAsteroids = 0;
            for (auto e : entities)
                if (e->name == "asteroid") countAsteroids++;

            while (countAsteroids < MIN_ASTEROIDS) {
                asteroid* a = new asteroid();
                float x, y;
                do {
                    x = rand() % W;
                    y = rand() % H;
                } while (std::abs(x - p->x) < 100 && std::abs(y - p->y) < 100);

                a->settings(sRock, x, y, rand() % 360, 25.f);
                float speed = ((rand() % 3 + 1) * 0.5f) * 3.f;
                a->dx = cos(a->angle * 3.14159f / 180.f) * speed;
                a->dy = sin(a->angle * 3.14159f / 180.f) * speed;

                entities.push_back(a);
                countAsteroids++;
            }

            for (auto i = entities.begin(); i != entities.end();) {
                Entity* e = *i;
                e->update();
                e->anim.update();
                if (!e->life) { i = entities.erase(i); delete e; }
                else ++i;
            }
        }

        // ===== DRAWING (nằm bên trong vòng while app.isOpen()) =====
        app.clear();

        if (currentState == GameState::MENU) {
            menu.draw(app);
        }
        else if (currentState == GameState::PLAYING) {
            app.draw(background);
            for (auto e : entities) e->draw(app);
            display.draw(app);
        }
        else if (currentState == GameState::PAUSED) {
            app.draw(background);
            for (auto e : entities) e->draw(app);
            pauseMenu.draw(app);
        }
        else if (currentState == GameState::GAME_OVER) {
            gameOverMenu.draw(app);

            // Hiển thị điểm số
            sf::Text finalScoreText;
            finalScoreText.setFont(font); // font đã load trước đó
            finalScoreText.setCharacterSize(36);
            finalScoreText.setFillColor(sf::Color::Yellow);
            finalScoreText.setString("Your Score: " + std::to_string(display.score));
            finalScoreText.setPosition(W / 2.f - finalScoreText.getGlobalBounds().width / 2.f, 150);
            app.draw(finalScoreText);

            // Gợi ý phím
            sf::Text hintText;
            hintText.setFont(font);
            hintText.setCharacterSize(20);
            hintText.setFillColor(sf::Color(200, 200, 200));
            hintText.setPosition(W / 2.f - hintText.getGlobalBounds().width / 2.f, H - 60);
            app.draw(hintText);

            // Ghi điểm vào file nếu chưa ghi
            if (!scoreSaved) {
                HighScore hs("highscores.txt");
                hs.loadScores();

                // Lấy số lượng điểm hiện có
                int nextRank = hs.getTopScores(1000).size() + 1;

                // Thêm điểm mới với tên tùy chỉnh
                std::string playerName = "Player " + std::to_string(nextRank); // hoặc "You"
                hs.addScore(playerName, display.score);

                hs.saveScores();

                scoreSaved = true;
            }

        }

        else if (currentState == GameState::HIGH_SCORES) {
            app.clear();

            HighScore manager("highscores.txt");
            manager.addScore("Player", display.score);  // currentScore là int bạn đã tính trước đó
            auto topScores = manager.getTopScores(10);  // Lấy top 10

            sf::Text title("Top 10 High Scores", font, 40);
            title.setFillColor(sf::Color::White);
            title.setPosition(W / 2.f - title.getGlobalBounds().width / 2.f, 50);
            app.draw(title);

            for (size_t i = 0; i < topScores.size(); ++i) {
                std::string line = "Top " + std::to_string(i + 1) + ": " +
                    topScores[i].first + " - " +
                    std::to_string(topScores[i].second);

                sf::Text scoreText(line, font, 28);
                scoreText.setFillColor(sf::Color::Yellow);
                scoreText.setPosition(W / 2.f - scoreText.getGlobalBounds().width / 2.f, 120 + i * 35);
                app.draw(scoreText);
            }




            sf::Text hint("Press Escape to return", font, 20);
            hint.setFillColor(sf::Color(200, 200, 200));
            hint.setPosition(W / 2.f - hint.getGlobalBounds().width / 2.f, H - 60);
            app.draw(hint);

        }
        app.display();
    }

    // Cleanup
    for (auto e : entities) delete e;
    entities.clear();

    return 0;
}
