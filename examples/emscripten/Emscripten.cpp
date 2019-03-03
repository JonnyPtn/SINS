////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <emscripten.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <cmath>

namespace
{
    // Define some constants
    const float pi = 3.14159f;
    const int gameWidth = 800;
    const int gameHeight = 600;
    sf::Vector2f paddleSize(25, 100);
    float ballRadius = 10.f;

    std::unique_ptr<sf::RenderWindow> window;
    std::unique_ptr<sf::Font> font;
    std::unique_ptr<sf::SoundBuffer> soundBuffer;
    std::unique_ptr<sf::Sound> sound;
    sf::RectangleShape leftPaddle;
    sf::RectangleShape rightPaddle;
    sf::CircleShape ball;
    sf::Text pauseMessage;

    // Define the paddles properties
    sf::Clock AITimer;
    const sf::Time AITime   = sf::seconds(0.1f);
    const float paddleSpeed = 400.f;
    float rightPaddleSpeed  = 0.f;
    const float ballSpeed   = 400.f;
    float ballAngle         = 0.f; // to be changed later

    sf::Clock dtClock;
    bool isPlaying{};
    bool upPressed{};
    bool downPressed{};
}

void emscriptenMain()
{
    if (!window)
    {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        // Create the window of the application
        window = std::make_unique<sf::RenderWindow>(sf::VideoMode(gameWidth, gameHeight, 32), "SFML Pong");

        // Setup the left paddle
        leftPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
        leftPaddle.setOutlineThickness(3);
        leftPaddle.setOutlineColor(sf::Color::Black);
        leftPaddle.setFillColor(sf::Color(100, 100, 200));
        leftPaddle.setOrigin(paddleSize / 2.f);

        // Setup the right paddle
        rightPaddle.setSize(paddleSize - sf::Vector2f(3, 3));
        rightPaddle.setOutlineThickness(3);
        rightPaddle.setOutlineColor(sf::Color::Black);
        rightPaddle.setFillColor(sf::Color(200, 100, 100));
        rightPaddle.setOrigin(paddleSize / 2.f);

        // Setup the ball
        ball.setRadius(ballRadius - 3);
        ball.setOutlineThickness(3);
        ball.setOutlineColor(sf::Color::Black);
        ball.setFillColor(sf::Color::White);
        ball.setOrigin(ballRadius / 2, ballRadius / 2);

        font = std::make_unique<sf::Font>();
        // Load the text font
        font->loadFromFile("sansation.ttf");

        // Load the sound
        sound = std::make_unique<sf::Sound>();
        soundBuffer = std::make_unique<sf::SoundBuffer>();
        if (soundBuffer->loadFromFile("ball.wav"))
        {
            sound->setBuffer(*soundBuffer);
        }

        // Initialize the pause message
        pauseMessage.setFont(*font);
        pauseMessage.setCharacterSize(40);
        pauseMessage.setPosition(170.f, 150.f);
        pauseMessage.setFillColor(sf::Color::White);
    
        pauseMessage.setString("Welcome to SFML pong!\nPress space to start the game");
    }
    if (window->isOpen())
    {
        // Handle events
        sf::Event event;
        while (window->pollEvent(event))
        {
            // Window closed or escape key pressed: exit
            if ((event.type == sf::Event::Type::Closed) ||
               ((event.type == sf::Event::Type::KeyPressed) && (event.key.code == sf::Keyboard::Key::Escape)))
            {
                window->close();
                break;
            }

            if (event.type == sf::Event::KeyPressed)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Key::Space:
                    if (!isPlaying)
                    {
                        // (re)start the game
                        isPlaying = true;
                        dtClock.restart();

                        // Reset the position of the paddles and ball
                        leftPaddle.setPosition(10 + paddleSize.x / 2, gameHeight / 2);
                        rightPaddle.setPosition(gameWidth - 10 - paddleSize.x / 2, gameHeight / 2);
                        ball.setPosition(gameWidth / 2, gameHeight / 2);

                        // Reset the ball angle
                        do
                        {
                            // Make sure the ball initial angle is not too much vertical
                            ballAngle = (std::rand() % 360) * 2 * pi / 360;
                        }
                        while (std::abs(std::cos(ballAngle)) < 0.7f);
                    }
                    break;
                    case sf::Keyboard::Key::Up:
                        upPressed = true;
                        break;
                    case sf::Keyboard::Key::Down:
                        downPressed = true;
                        break;
                }
            }
            if (event.type == sf::Event::KeyReleased)
            {
                switch (event.key.code)
                {
                    case sf::Keyboard::Key::Up:
                        upPressed = false;
                        break;
                    case sf::Keyboard::Key::Down:
                        downPressed = false;
                        break;
                }
            }
            
            // Window size changed, adjust view appropriately
            if (event.type == sf::Event::Resized)
            {
                sf::View view;
                view.setSize(gameWidth, gameHeight);
                view.setCenter(gameWidth/2.f, gameHeight/2.f);
                window->setView(view);
            }
        }

        if (isPlaying)
        {
            float deltaTime = dtClock.restart().asSeconds();

            // Move the player's paddle
            if (upPressed && (leftPaddle.getPosition().y - paddleSize.y / 2 > 5.f))
            {
                leftPaddle.move(0.f, -paddleSpeed * deltaTime);
            }
            if (downPressed && (leftPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f))
            {
                leftPaddle.move(0.f, paddleSpeed * deltaTime);
            }

            // Move the computer's paddle
            if (((rightPaddleSpeed < 0.f) && (rightPaddle.getPosition().y - paddleSize.y / 2 > 5.f)) ||
                ((rightPaddleSpeed > 0.f) && (rightPaddle.getPosition().y + paddleSize.y / 2 < gameHeight - 5.f)))
            {
                rightPaddle.move(0.f, rightPaddleSpeed * deltaTime);
            }

            // Update the computer's paddle direction according to the ball position
            if (AITimer.getElapsedTime() > AITime)
            {
                AITimer.restart();
                if (ball.getPosition().y + ballRadius > rightPaddle.getPosition().y + paddleSize.y / 2)
                    rightPaddleSpeed = paddleSpeed;
                else if (ball.getPosition().y - ballRadius < rightPaddle.getPosition().y - paddleSize.y / 2)
                    rightPaddleSpeed = -paddleSpeed;
                else
                    rightPaddleSpeed = 0.f;
            }

            // Move the ball
            float factor = ballSpeed * deltaTime;
            ball.move(std::cos(ballAngle) * factor, std::sin(ballAngle) * factor);

            const std::string inputString = "Press space to restart";
            
            // Check collisions between the ball and the screen
            if (ball.getPosition().x - ballRadius < 0.f)
            {
                isPlaying = false;
                pauseMessage.setString("You Lost!\n" + inputString);
            }
            if (ball.getPosition().x + ballRadius > gameWidth)
            {
                isPlaying = false;
                pauseMessage.setString("You Won!\n" + inputString);
            }
            if (ball.getPosition().y - ballRadius < 0.f)
            {
                sound->play();
                ballAngle = -ballAngle;
                ball.setPosition(ball.getPosition().x, ballRadius + 0.1f);
            }
            if (ball.getPosition().y + ballRadius > gameHeight)
            {
                sound->play();
                ballAngle = -ballAngle;
                ball.setPosition(ball.getPosition().x, gameHeight - ballRadius - 0.1f);
            }

            // Check the collisions between the ball and the paddles
            // Left Paddle
            if (ball.getPosition().x - ballRadius < leftPaddle.getPosition().x + paddleSize.x / 2 &&
                ball.getPosition().x - ballRadius > leftPaddle.getPosition().x &&
                ball.getPosition().y + ballRadius >= leftPaddle.getPosition().y - paddleSize.y / 2 &&
                ball.getPosition().y - ballRadius <= leftPaddle.getPosition().y + paddleSize.y / 2)
            {
                sound->play();
                if (ball.getPosition().y > leftPaddle.getPosition().y)
                    ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
                else
                    ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;

                ball.setPosition(leftPaddle.getPosition().x + ballRadius + paddleSize.x / 2 + 0.1f, ball.getPosition().y);
            }

            // Right Paddle
            if (ball.getPosition().x + ballRadius > rightPaddle.getPosition().x - paddleSize.x / 2 &&
                ball.getPosition().x + ballRadius < rightPaddle.getPosition().x &&
                ball.getPosition().y + ballRadius >= rightPaddle.getPosition().y - paddleSize.y / 2 &&
                ball.getPosition().y - ballRadius <= rightPaddle.getPosition().y + paddleSize.y / 2)
            {
                sound->play();
                if (ball.getPosition().y > rightPaddle.getPosition().y)
                    ballAngle = pi - ballAngle + (std::rand() % 20) * pi / 180;
                else
                    ballAngle = pi - ballAngle - (std::rand() % 20) * pi / 180;

                ball.setPosition(rightPaddle.getPosition().x - ballRadius - paddleSize.x / 2 - 0.1f, ball.getPosition().y);
            }
        }

        // Clear the window
        window->clear(sf::Color(50, 200, 50));

        if (isPlaying)
        {
            // Draw the paddles and the ball
            window->draw(leftPaddle);
            window->draw(rightPaddle);
            window->draw(ball);
        }
        else
        {
            // Draw the pause message
            window->draw(pauseMessage);
        }

        // Display things on screen
        window->display();
    }
}

int main()
{
    emscripten_set_main_loop(&emscriptenMain, 0, true);
}
