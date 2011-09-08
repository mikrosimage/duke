/*
 * SfmlWindow.cpp
 *
 *  Created on: 5 juil. 2010
 *      Author: Guillaume Chatelet
 */

#include "SfmlWindow.h"
#include <renderer/common/RendererSuite.h>
#include <SFML/Window.hpp>
#include <stdexcept>
#include <iostream>
#include <cassert>

using namespace std;

SfmlWindow::SfmlWindow(const RendererFactoryFunc& rendererFactoryFunction, protocol::duke::Renderer renderer, const RendererSuite& suite) :
    m_bWindowCreated(false) {
    try {
        const sf::VideoMode desktopMode = sf::VideoMode::GetDesktopMode();
        unsigned long windowStyle = 0;
        if (renderer.fullscreen())
            renderer.set_decoratewindow(false);
        if (renderer.decoratewindow()) {
            windowStyle |= sf::Style::Resize;
            windowStyle |= sf::Style::Close;
        }
        if (renderer.fullscreen()) {
            /**
             * we don't want to set the fullscreen flag here
             * because it will trigger a WindowImplWin32::SwitchToFullscreen()
             * which will bring the refresh rate back to 60Hz
             * and prevent correct synchronization for playback
             */
            // windowStyle |= sf::Style::Fullscreen;
            renderer.set_width(desktopMode.Width);
            renderer.set_height(desktopMode.Height);
        }
        // creating the window
        if (renderer.has_handle() && renderer.handle() != 0) {
#if defined(_WIN32) || defined(__WIN32__) || defined(__APPLE__)
            sf::ContextSettings settings;
            m_Window.Create((HWND__*) renderer.handle(), settings);
#else
            m_Window.Create( renderer.handle() );
#endif
        } else {
            m_Window.Create(sf::VideoMode(renderer.width(), renderer.height(), desktopMode.BitsPerPixel), SFML_WINDOW_TITLE, windowStyle);
        }

        // initializing the renderer
        m_pRenderer.reset(rendererFactoryFunction(renderer, m_Window, suite));
        assert( m_pRenderer.get() );

        // engaging the rendering loop
        m_pRenderer->loop();
    } catch (exception& ex) {
        cerr << ex.what() << endl;
    }
}

SfmlWindow::~SfmlWindow() {
    m_MainLoopThread.join();
}

void SfmlWindow::waitForMainLoopEnd() {
    boost::lock_guard<boost::mutex> lock(m_Mutex);
    m_MainLoopThread.join();
}

