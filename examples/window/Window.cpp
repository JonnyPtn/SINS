////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window.hpp>
#include <iostream>


////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
    // Create the main window
    sf::Window window(sf::VideoMode(640, 480), "SFML window", sf::Style::Default);

    std::cout << "Window created, system handle: " << window.getSystemHandle() << std::endl;

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
            default:
            {
                std::cout << "Some other mysterious event..." << std::endl;
            }
            }
        }
    }

    return 0;
}
