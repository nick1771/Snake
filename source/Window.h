#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <queue>
#include <string>

#include "Primitives.h"
#include "Event.h"

struct WindowProperties {
    std::string title = "Window";

    i32 width = 800;
    i32 height = 600;

    bool isVisible = true;
};

namespace detail {

    struct WindowState {
        std::queue<Event> pendingEvents{};
        WindowProperties properties{};
        bool isCloseRequested{};
    };
}

class Window {
public:
    Window(const WindowProperties& properties);

    void destroy();
    void blit(const u8* data);

    void setVisible(bool isVisible);
    void setSize(i32 width, i32 height);

    i32 getWidth() const;
    i32 getHeight() const;
    bool getEvent(Event& event);

    bool isCloseRequested() const;
private:
    void _pollEvents();

    detail::WindowState _state{};
    HWND _handle{};
};
