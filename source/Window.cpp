#include "Window.h"

#include <windowsx.h>
#include <stdexcept>

namespace {

	constexpr std::wstring_view CLASS_NAME = L"WINDOW_CLASS";

	Key mapWindowsKeyToEnum(WPARAM key) {
		switch (key) {
		case VK_ESCAPE:
			return Key::Escape;
		case 0x41:
			return Key::A;
		case 0x44:
			return Key::D;
		case 0x53:
			return Key::S;
		case 0x57:
			return Key::W;
		case VK_SPACE:
			return Key::Space;
		default:
			return Key::Unhandled;
		}
	}

	std::wstring convertStringToWideString(const std::string& string) {
		if (string.empty()) {
			return L"";
		}

		auto wideStringSize = MultiByteToWideChar(
			CP_UTF8,
			0,
			string.data(),
			static_cast<int>(string.size()),
			nullptr,
			0
		);

		if (wideStringSize <= 0) {
			throw std::runtime_error("MultiByteToWideChar() failed");
		}

		auto result = std::wstring(wideStringSize, 0);
		auto resultCode = MultiByteToWideChar(
			CP_UTF8,
			0,
			string.data(),
			static_cast<int>(string.size()),
			result.data(),
			wideStringSize
		);

		if (resultCode <= 0) {
			throw std::runtime_error("MultiByteToWideChar() failed");
		}

		return result;
	}

	LRESULT onWindowEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
		if (msg == WM_CREATE) {
			auto createStruct = reinterpret_cast<CREATESTRUCT*>(lparam);
			auto windowState = reinterpret_cast<LONG_PTR>(createStruct->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, windowState);
		}

		auto windowState = reinterpret_cast<detail::WindowState*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (windowState == nullptr) {
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}

		switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			windowState->isCloseRequested = true;
			break;
		case WM_CLOSE:
			windowState->pendingEvents.emplace(WindowClosed{});
			break;
		case WM_KEYDOWN:
			windowState->pendingEvents.emplace(KeyPressed{ mapWindowsKeyToEnum(wparam) });
			break;
		case WM_KEYUP:
			windowState->pendingEvents.emplace(KeyReleased{ mapWindowsKeyToEnum(wparam) });
			break;
		case WM_MOUSEMOVE:
			windowState->pendingEvents.emplace(MouseMove{ GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) });
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}

		return 0;
	}
}

Window::Window(const WindowProperties& properties) {
	_state.properties = properties;

	auto instanceHandle = GetModuleHandle(nullptr);

	auto windowClass = WNDCLASSW{};
	windowClass.hInstance = instanceHandle;
	windowClass.lpszClassName = CLASS_NAME.data();
	windowClass.lpfnWndProc = onWindowEvent;
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&windowClass);

	auto windowTitle = convertStringToWideString(_state.properties.title);
	auto windowStyle = WS_OVERLAPPED | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU;

	RECT windowClientArea{};
	windowClientArea.left = 0;
	windowClientArea.top = 0;
	windowClientArea.right = properties.width;
	windowClientArea.bottom = properties.height;

	AdjustWindowRect(&windowClientArea, windowStyle, false);

	_handle = CreateWindow(
		CLASS_NAME.data(),
		windowTitle.data(),
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowClientArea.right - windowClientArea.left,
		windowClientArea.bottom - windowClientArea.top,
		nullptr,
		nullptr,
		instanceHandle,
		static_cast<void*>(&_state)
	);

	if (_state.properties.isVisible) {
		setVisible(true);
	}
}

void Window::destroy() {
	DestroyWindow(_handle);
}

void Window::blit(const u8* data) {
	auto bitmapDescription = BITMAPINFO{};
	bitmapDescription.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapDescription.bmiHeader.biWidth = _state.properties.width;
	bitmapDescription.bmiHeader.biHeight = -static_cast<i32>(_state.properties.height);
	bitmapDescription.bmiHeader.biPlanes = 1;
	bitmapDescription.bmiHeader.biBitCount = 32;
	bitmapDescription.bmiHeader.biCompression = BI_RGB;

	auto deviceContext = GetDC(_handle);

	StretchDIBits(
		deviceContext,
		0,
		0,
		_state.properties.width,
		_state.properties.height,
		0,
		0,
		_state.properties.width,
		_state.properties.height,
		data,
		&bitmapDescription,
		DIB_RGB_COLORS,
		SRCCOPY
	);

	ReleaseDC(_handle, deviceContext);
}

void Window::setVisible(bool isVisible) {
	_state.properties.isVisible = isVisible;
	auto flag = isVisible ? SW_SHOW : SW_SHOW;
	ShowWindow(_handle, flag);
}

void Window::setSize(i32 width, i32 height) {
	auto requiredClientRect = RECT{ 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
	AdjustWindowRect(&requiredClientRect, WS_OVERLAPPEDWINDOW, false);

	auto windowWidth = requiredClientRect.right - requiredClientRect.left;
	auto windowHeight = requiredClientRect.bottom - requiredClientRect.top;
	SetWindowPos(_handle, nullptr, 0, 0, windowWidth, windowHeight, SWP_NOMOVE);

	_state.properties.width = width;
	_state.properties.height = height;
}

i32 Window::getWidth() const {
	return _state.properties.width;
}

i32 Window::getHeight() const {
	return _state.properties.height;
}

bool Window::getEvent(Event& event) {
	if (_state.pendingEvents.empty()) {
		_pollEvents();
	}

	if (!_state.pendingEvents.empty()) {
		event = _state.pendingEvents.front();
		_state.pendingEvents.pop();
		return true;
	}

	return false;
}

bool Window::isCloseRequested() const {
	return _state.isCloseRequested;
}

void Window::_pollEvents() {
	auto message = MSG{};
	while (PeekMessageW(&message, 0, 0, 0, PM_REMOVE) != 0) {
		TranslateMessage(&message);
		DispatchMessageW(&message);
	}
}
