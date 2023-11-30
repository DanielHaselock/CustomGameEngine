#include "EngineWindow/Core/Window.h"
#include <cstdlib>
#include <mutex>
#include "Rendering/Renderer/VK/VKRenderer.h"
#include "EngineCore/EventSystem/InputManager.h"
#ifdef NSHIPPING
#include "Editor/Widget/WidgetGameApp.h"
#endif
using namespace EngineWindow::Core;

int Window::mInstance = 0;
std::mutex Window::mInstanceLock;

void Window::framebufferSizeCallback(GLFWwindow* pWindow, int pWidth, int pHeight)
{
	Rendering::Renderer::VK::VKRenderer* renderer = (Rendering::Renderer::VK::VKRenderer*)glfwGetWindowUserPointer(pWindow);
	if (renderer != nullptr)
		renderer->mFramebufferResized = true;
}

void Window::mouseButton(GLFWwindow* pWindow, int pButton, int pAction, int pMods)
{
	Rendering::Renderer::VK::VKRenderer* renderer = (Rendering::Renderer::VK::VKRenderer*)glfwGetWindowUserPointer(pWindow);
	renderer->mWindow->press = pButton == GLFW_MOUSE_BUTTON_LEFT && pAction == GLFW_PRESS;
#ifdef NSHIPPING
	renderer->mWindow->changeCursorMode(renderer->mWindow->mOldState);
#endif
	if (renderer->mWindow->mInput == nullptr)
		return;

	if (pButton == GLFW_MOUSE_BUTTON_LEFT)
	{
		renderer->mWindow->mInput->receiveMouseInput(pAction == GLFW_PRESS ? EngineCore::EventSystem::KState::PRESS : EngineCore::EventSystem::KState::RELEASE
			, EngineCore::EventSystem::MouseKey::LEFT);
	}
	else if (pButton == GLFW_MOUSE_BUTTON_RIGHT)
		renderer->mWindow->mInput->receiveMouseInput(pAction == GLFW_PRESS ? EngineCore::EventSystem::KState::PRESS : EngineCore::EventSystem::KState::RELEASE
			, EngineCore::EventSystem::MouseKey::RIGHT);

}

void Window::cursorPosition(GLFWwindow* window, double xpos, double ypos)
{
	Rendering::Renderer::VK::VKRenderer* renderer = (Rendering::Renderer::VK::VKRenderer*)glfwGetWindowUserPointer(window);
	renderer->mWindow->mousePos.x = xpos;
	renderer->mWindow->mousePos.y = ypos;
}

void Window::scrollCallback(GLFWwindow* pWindow, double /*pX*/, double pY)
{
	Rendering::Renderer::VK::VKRenderer* renderer = (Rendering::Renderer::VK::VKRenderer*)glfwGetWindowUserPointer(pWindow);
	if (renderer->mWindow->mInput != nullptr)
		renderer->mWindow->mInput->receiveMouseScroll(pY);
}

void Window::keyCallback(GLFWwindow* pWindow, int pKey, int pScancode, int pAction, int pMods)
{
	Rendering::Renderer::VK::VKRenderer* renderer = (Rendering::Renderer::VK::VKRenderer*)glfwGetWindowUserPointer(pWindow);
	if (renderer->mWindow->mInput == nullptr)
		return;
	EngineCore::EventSystem::Key key;
	key = getKey(pKey);

	renderer->mWindow->mInput->receiveInput(pAction == GLFW_PRESS || pAction == GLFW_REPEAT ? EngineCore::EventSystem::KState::PRESS : EngineCore::EventSystem::KState::RELEASE, key);

}

EngineCore::EventSystem::Key Window::getKey(int pKey)
{
	switch (pKey)
	{
	case GLFW_KEY_A:
	{
		return EngineCore::EventSystem::Key::A;
	}
	case GLFW_KEY_B:
	{
		return EngineCore::EventSystem::Key::B;
	}
	case GLFW_KEY_C:
	{
		return EngineCore::EventSystem::Key::C;
	}
	case GLFW_KEY_D:
	{
		return EngineCore::EventSystem::Key::D;
	}
	case GLFW_KEY_E:
	{
		return EngineCore::EventSystem::Key::E;
	}
	case GLFW_KEY_F:
	{
		return EngineCore::EventSystem::Key::F;
	}
	case GLFW_KEY_G:
	{
		return EngineCore::EventSystem::Key::G;
	}
	case GLFW_KEY_H:
	{
		return EngineCore::EventSystem::Key::H;
	}
	case GLFW_KEY_I:
	{
		return EngineCore::EventSystem::Key::I;
	}
	case GLFW_KEY_J:
	{
		return EngineCore::EventSystem::Key::J;
	}
	case GLFW_KEY_K:
	{
		return EngineCore::EventSystem::Key::K;
	}
	case GLFW_KEY_L:
	{
		return EngineCore::EventSystem::Key::L;
	}
	case GLFW_KEY_M:
	{
		return EngineCore::EventSystem::Key::M;
	}
	case GLFW_KEY_N:
	{
		return EngineCore::EventSystem::Key::N;
	}
	case GLFW_KEY_O:
	{
		return EngineCore::EventSystem::Key::O;
	}
	case GLFW_KEY_P:
	{
		return EngineCore::EventSystem::Key::P;
	}
	case GLFW_KEY_Q:
	{
		return EngineCore::EventSystem::Key::Q;
	}
	case GLFW_KEY_R:
	{
		return EngineCore::EventSystem::Key::R;
	}
	case GLFW_KEY_S:
	{
		return EngineCore::EventSystem::Key::S;
	}
	case GLFW_KEY_T:
	{
		return EngineCore::EventSystem::Key::T;
	}
	case GLFW_KEY_U:
	{
		return EngineCore::EventSystem::Key::U;
	}
	case GLFW_KEY_V:
	{
		return EngineCore::EventSystem::Key::V;
	}
	case GLFW_KEY_W:
	{
		return EngineCore::EventSystem::Key::W;
	}
	case GLFW_KEY_X:
	{
		return EngineCore::EventSystem::Key::X;
	}
	case GLFW_KEY_Y:
	{
		return EngineCore::EventSystem::Key::Y;
	}
	case GLFW_KEY_Z:
	{
		return EngineCore::EventSystem::Key::Z;
	}
	case GLFW_KEY_1:
	{
		return EngineCore::EventSystem::Key::_1;
	}
	case GLFW_KEY_2:
	{
		return EngineCore::EventSystem::Key::_2;
	}
	case GLFW_KEY_3:
	{
		return EngineCore::EventSystem::Key::_3;
	}
	case GLFW_KEY_4:
	{
		return EngineCore::EventSystem::Key::_4;
	}
	case GLFW_KEY_5:
	{
		return EngineCore::EventSystem::Key::_5;
	}
	case GLFW_KEY_6:
	{
		return EngineCore::EventSystem::Key::_6;
	}
	case GLFW_KEY_7:
	{
		return EngineCore::EventSystem::Key::_7;
	}
	case GLFW_KEY_8:
	{
		return EngineCore::EventSystem::Key::_8;
	}
	case GLFW_KEY_9:
	{
		return EngineCore::EventSystem::Key::_9;
	}
	case GLFW_KEY_TAB:
	{
		return EngineCore::EventSystem::Key::TAB;
	}
	case GLFW_KEY_LEFT_CONTROL:
	{
		return EngineCore::EventSystem::Key::LEFT_CTRL;
	}
	case GLFW_KEY_LEFT_SHIFT:
	{
		return EngineCore::EventSystem::Key::LEFT_SHIFT;
	}
	case GLFW_KEY_ENTER:
	{
		return EngineCore::EventSystem::Key::ENTER;
	}
	case GLFW_KEY_ESCAPE:
	{
		return EngineCore::EventSystem::Key::ESCAPE;
	}
	case GLFW_KEY_SPACE:
	{
		return EngineCore::EventSystem::Key::SPACE;
	}
	default:
		return EngineCore::EventSystem::Key::UNKNOWN;
	}
}

Window::Window()
{
}

Window::Window(const char* pTitle, const unsigned int& pWidth, const unsigned int& pHeight) : mWidth(pWidth), mHeight(pHeight)
{
	{
		std::unique_lock lock(mInstanceLock);
		if (mInstance == 0)
			glfwInit();

		mInstance++;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	#ifdef SHIPPING
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	#else
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	#endif

	


	mWindow = glfwCreateWindow(pWidth, pHeight, pTitle, nullptr, nullptr);
	if (mWindow == nullptr)
	{
		std::unique_lock lock(mInstanceLock);
		if (mInstance == 1)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		else
		{
			mInstance--;
			return;
		}
	}

	// Center window
	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	if (primary != nullptr)
	{
		const GLFWvidmode* desktop = glfwGetVideoMode(primary);
		glfwSetWindowPos(mWindow, (desktop->width - pWidth) / 2, (desktop->height - pHeight) / 2);
	}

	glfwMakeContextCurrent(mWindow);
	glfwSetFramebufferSizeCallback(mWindow, Window::framebufferSizeCallback);
	glfwSetMouseButtonCallback(mWindow, Window::mouseButton);
	glfwSetCursorPosCallback(mWindow, Window::cursorPosition);
	glfwSetKeyCallback(mWindow, Window::keyCallback);
	glfwSetScrollCallback(mWindow, Window::scrollCallback);
}

Window::~Window()
{
	if (!mShutdown)
		shutDown();
}

void Window::close()
{
	glfwSetWindowShouldClose(mWindow, true);
}

void Window::shutDown()
{
	glfwDestroyWindow(mWindow);
	mShutdown = true;

	{
		std::unique_lock lock(mInstanceLock);
		mInstance--;

		if (mInstance == 0)
			glfwTerminate();
	}
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(mWindow);
}

void Window::pollEvents()
{
	glfwPollEvents();
}

void Window::waitEvent()
{
	glfwWaitEvents();
}

void Window::setWindowUserPointer(void* pointer)
{
	glfwSetWindowUserPointer(mWindow, pointer);
}

int Window::getKey(const unsigned int& key)
{
	return glfwGetKey(mWindow, key);
}

void Window::bindContext(bool pBinded)
{
	if (pBinded)
		glfwMakeContextCurrent(mWindow);
	else
		glfwMakeContextCurrent(nullptr);
}

void Window::setInputManager(EngineCore::EventSystem::InputManager* pInput)
{
	mInput = pInput;
}

const char** Window::getRequiredInstanceExtensions(uint32_t& pExtensionCount)
{
	return glfwGetRequiredInstanceExtensions(&pExtensionCount);
}

void Window::getFramebufferSize(int* pWidth, int* pHeight)
{
	glfwGetFramebufferSize(mWindow, pWidth, pHeight);

	mWidth = *pWidth;
	mHeight = *pHeight;
}

void Window::updateFramebufferSize()
{
	glfwGetFramebufferSize(mWindow, &mWidth, &mHeight);
}

HWND Window::Window::getWindowId()
{
	return glfwGetWin32Window(mWindow);
}

void Window::changeCursorMode(bool pShowHide, bool pSaveState)
{
	glfwSetInputMode(mWindow, GLFW_CURSOR, pShowHide ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
	if(pSaveState)
		mOldState = pShowHide;
}

void Window::setMousePosition(Maths::FVector2& pPos)
{
	glfwSetCursorPos(mWindow, pPos.x, pPos.y);
}

bool Window::IsActiveWindow()
{
	return getWindowId() == GetActiveWindow();
}
