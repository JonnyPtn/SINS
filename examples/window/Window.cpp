////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window.hpp>
#include <iostream>

#ifdef SFML_SYSTEM_IOS
#include <SFML/Main.hpp>
#endif

#ifdef SFML_OPENGL_ES
#define glClearDepth glClearDepthf
#define glFrustum glFrustumf
#endif

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
    // Create the main window
    const auto title = "SFML window";
    std::uint8_t style = sf::Style::Default;
    const auto width = 640u, height = 480u, bpp = 32u;
    sf::Window window({ width, height, bpp }, title, style);

    std::cout << "Window created, system handle: " << window.getSystemHandle() << std::endl;
    std::cout << "Arrows move the window. Press F to toggle full screen" << std::endl;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Type::Closed:
            {
                window.close();
                break;
            }
            case sf::Event::Type::Resized:
            {
                auto size = event.size;
                std::cout << "Window resized to " << size.width << "," << size.height << std::endl;

                glViewport(0, 0, event.size.width, event.size.height);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                GLfloat ratio = static_cast<float>(event.size.width) / event.size.height;
                glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);
                break;
            }
            case sf::Event::Type::LostFocus:
            {
                std::cout << "Focus lost" << std::endl;
                break;
            }
            case sf::Event::Type::GainedFocus:
            {
                std::cout << "Focus gained" << std::endl;
                break;
            }
            case sf::Event::Type::KeyPressed:
            {
                const auto pos = window.getPosition();
                const auto movement = 10;
                switch (event.key.code)
                {
                    case sf::Keyboard::Key::Left:
                    {
                        window.setPosition({ pos.x - movement, pos.y });
                        break;
                    }
                    case sf::Keyboard::Key::Right:
                    {
                        window.setPosition({ pos.x + movement, pos.y });
                        break;
                    }
                    case sf::Keyboard::Key::Up:
                    {
                        window.setPosition({ pos.x, pos.y - movement });
                        break;
                    }
                    case sf::Keyboard::Key::Down:
                    {
                        window.setPosition({ pos.x, pos.y + movement });
                        break;
                    }
                    case sf::Keyboard::Key::F:
                    {
                        style ^= sf::Style::Fullscreen;
                        auto size = window.getSize();
                        window.create({ size.x, size.y, bpp }, title, style);
                    }
                }
            }
            default:
                break;
            }
        }
    }

    return 0;
}
