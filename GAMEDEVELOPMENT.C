//GAME DEVELOPMENT: SNAKE GAME
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 600;
const int SIZE = 20; // Size of snake segment and food

enum Direction { Up, Down, Left, Right };

struct SnakeSegment {
    int x, y;
    SnakeSegment(int x, int y) : x(x), y(y) {}
};

class SnakeGame {
    sf::RenderWindow window;
    sf::Font font;
    sf::Text scoreText, gameOverText;
    sf::SoundBuffer eatBuffer, gameOverBuffer;
    sf::Sound eatSound, gameOverSound;

    std::vector<SnakeSegment> snake;
    Direction dir;
    int foodX, foodY;
    int score;
    float speed;
    bool gameOver;

public:
    SnakeGame()
        : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake Game - SFML"),
          dir(Right), score(0), speed(0.15f), gameOver(false)
    {
        window.setFramerateLimit(60);
        srand(static_cast<unsigned>(time(0)));
        snake.push_back(SnakeSegment(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));
        placeFood();

        font.loadFromFile("assets/font.ttf");
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::White);

        gameOverText.setFont(font);
        gameOverText.setCharacterSize(48);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setString("GAME OVER\nPress R to Restart");
        gameOverText.setPosition(60, WINDOW_HEIGHT/2 - 60);

        eatBuffer.loadFromFile("assets/eat.wav");
        eatSound.setBuffer(eatBuffer);

        gameOverBuffer.loadFromFile("assets/gameover.wav");
        gameOverSound.setBuffer(gameOverBuffer);
    }

    void run() {
        sf::Clock clock;
        float timer = 0;
        while (window.isOpen()) {
            float dt = clock.restart().asSeconds();
            timer += dt;
            handleInput();
            if (!gameOver && timer >= speed) {
                timer = 0;
                moveSnake();
            }
            render();
        }
    }

private:
    void placeFood() {
        foodX = (rand() % (WINDOW_WIDTH / SIZE)) * SIZE;
        foodY = (rand() % (WINDOW_HEIGHT / SIZE)) * SIZE;
    }

    void handleInput() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (!gameOver) {
                    if (event.key.code == sf::Keyboard::Up && dir != Down) dir = Up;
                    if (event.key.code == sf::Keyboard::Down && dir != Up) dir = Down;
                    if (event.key.code == sf::Keyboard::Left && dir != Right) dir = Left;
                    if (event.key.code == sf::Keyboard::Right && dir != Left) dir = Right;
                }
                if (gameOver && event.key.code == sf::Keyboard::R) restart();
            }
        }
    }

    void moveSnake() {
        // Move body
        for (int i = snake.size() - 1; i > 0; --i)
            snake[i] = snake[i-1];

        // Move head
        switch (dir) {
            case Up:    snake[0].y -= SIZE; break;
            case Down:  snake[0].y += SIZE; break;
            case Left:  snake[0].x -= SIZE; break;
            case Right: snake[0].x += SIZE; break;
        }

        // Wall collision
        if (snake[0].x < 0 || snake[0].x >= WINDOW_WIDTH || 
            snake[0].y < 0 || snake[0].y >= WINDOW_HEIGHT) {
            gameOver = true;
            gameOverSound.play();
        }

        // Self collision
        for (int i = 1; i < snake.size(); ++i) {
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                gameOver = true;
                gameOverSound.play();
            }
        }

        // Eating food
        if (snake[0].x == foodX && snake[0].y == foodY) {
            eatSound.play();
            snake.push_back(snake.back());
            score += 10;
            placeFood();
            // Increase speed (lower is faster)
            if (speed > 0.05f) speed -= 0.01f;
        }
    }

    void render() {
        window.clear(sf::Color(30,30,30));

        // Draw snake
        sf::RectangleShape rect(sf::Vector2f(SIZE-2, SIZE-2));
        rect.setFillColor(sf::Color::Green);
        for (auto& s : snake) {
            rect.setPosition(s.x+1, s.y+1);
            window.draw(rect);
        }

        // Draw food
        rect.setFillColor(sf::Color::Red);
        rect.setPosition(foodX+1, foodY+1);
        window.draw(rect);

        // Draw score
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setPosition(10, 10);
        window.draw(scoreText);

        // Draw game over
        if (gameOver)
            window.draw(gameOverText);

        window.display();
    }

    void restart() {
        snake.clear();
        snake.push_back(SnakeSegment(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));
        dir = Right;
        score = 0;
        speed = 0.15f;
        gameOver = false;
        placeFood();
    }
};

int main() {
    SnakeGame game;
    game.run();
    return 0;
}
