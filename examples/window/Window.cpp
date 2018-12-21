////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window.hpp>

#ifdef SFML_SYSTEM_WINDOWS
#include <windows.h>
#endif
#include <gl/GL.h>


////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
    // Create the main window
    sf::Window window(sf::VideoMode(640, 480), "SFML window with OpenGL", sf::Style::Default);

    // Create the openGL context
    // TODO other platforms
    auto dc = GetDC(window.getSystemHandle());
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
        PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
        32,                   // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                   // Number of bits for the depthbuffer
        8,                    // Number of bits for the stencilbuffer
        0,                    // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    auto pf = ChoosePixelFormat(dc, &pfd);
    SetPixelFormat(dc, pf, &pfd);
    auto context = wglCreateContext(dc);
    wglMakeCurrent(dc, context);

    // Set the color and depth clear values
    glClearDepth(1.f);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    // Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    // Disable lighting and texturing
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Configure the viewport (the same size as the window)
    glViewport(0, 0, window.getSize().x, window.getSize().y);

    // Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
    glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);

    // Define a 3D cube (6 faces made of 2 triangles composed by 3 vertices)
    GLfloat cube[] =
    {
        // positions    // colors (r, g, b, a)
        -50, -50, -50,  0, 0, 1, 1,
        -50,  50, -50,  0, 0, 1, 1,
        -50, -50,  50,  0, 0, 1, 1,
        -50, -50,  50,  0, 0, 1, 1,
        -50,  50, -50,  0, 0, 1, 1,
        -50,  50,  50,  0, 0, 1, 1,

         50, -50, -50,  0, 1, 0, 1,
         50,  50, -50,  0, 1, 0, 1,
         50, -50,  50,  0, 1, 0, 1,
         50, -50,  50,  0, 1, 0, 1,
         50,  50, -50,  0, 1, 0, 1,
         50,  50,  50,  0, 1, 0, 1,

        -50, -50, -50,  1, 0, 0, 1,
         50, -50, -50,  1, 0, 0, 1,
        -50, -50,  50,  1, 0, 0, 1,
        -50, -50,  50,  1, 0, 0, 1,
         50, -50, -50,  1, 0, 0, 1,
         50, -50,  50,  1, 0, 0, 1,

        -50,  50, -50,  0, 1, 1, 1,
         50,  50, -50,  0, 1, 1, 1,
        -50,  50,  50,  0, 1, 1, 1,
        -50,  50,  50,  0, 1, 1, 1,
         50,  50, -50,  0, 1, 1, 1,
         50,  50,  50,  0, 1, 1, 1,

        -50, -50, -50,  1, 0, 1, 1,
         50, -50, -50,  1, 0, 1, 1,
        -50,  50, -50,  1, 0, 1, 1,
        -50,  50, -50,  1, 0, 1, 1,
         50, -50, -50,  1, 0, 1, 1,
         50,  50, -50,  1, 0, 1, 1,

        -50, -50,  50,  1, 1, 0, 1,
         50, -50,  50,  1, 1, 0, 1,
        -50,  50,  50,  1, 1, 0, 1,
        -50,  50,  50,  1, 1, 0, 1,
         50, -50,  50,  1, 1, 0, 1,
         50,  50,  50,  1, 1, 0, 1,
    };

    // Enable position and color vertex components
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 7 * sizeof(GLfloat), cube);
    glColorPointer(4, GL_FLOAT, 7 * sizeof(GLfloat), cube + 3);

    // Disable normal and texture coordinates vertex components
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    // Create a clock for measuring the time elapsed
    sf::Clock clock;

    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Type::Closed)
                window.close();

            // Escape key: exit
            if ((event.type == sf::Event::Type::KeyPressed) && (event.key.code == sf::Keyboard::Key::Escape))
                window.close();

            // Resize event: adjust the viewport
            if (event.type == sf::Event::Type::Resized)
                glViewport(0, 0, event.size.width, event.size.height);
        }

        // Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Apply some transformations to rotate the cube
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.f, 0.f, -200.f);
        glRotatef(clock.getElapsedTime().asSeconds() * 50, 1.f, 0.f, 0.f);
        glRotatef(clock.getElapsedTime().asSeconds() * 30, 0.f, 1.f, 0.f);
        glRotatef(clock.getElapsedTime().asSeconds() * 90, 0.f, 0.f, 1.f);

        // Draw the cube
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // TODO: other platforms and better framerate control
        SwapBuffers(GetDC(window.getSystemHandle()));
        sf::sleep(sf::milliseconds(16));
    }

    return EXIT_SUCCESS;
}
