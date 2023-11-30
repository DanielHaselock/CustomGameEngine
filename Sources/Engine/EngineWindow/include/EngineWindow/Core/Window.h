#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#include <mutex>
#include "EngineCore/EventSystem/InputManager.h"

namespace EngineWindow::Core
{
	class Window
	{
	public:
		static int mInstance;
		static std::mutex mInstanceLock;

		GLFWwindow* mWindow;
		int mWidth;
		int mHeight;
		bool mShutdown = false;
		Maths::FVector2 mousePos;
		bool press = false;
		EngineCore::EventSystem::InputManager* mInput = nullptr;
		bool mOldState = true;

		static void framebufferSizeCallback(GLFWwindow* pWindow, int pWidth, int pHeight);
		static void mouseButton(GLFWwindow* pWindow, int pButton, int pAction, int pMods);
		static void cursorPosition(GLFWwindow* window, double xpos, double ypos);
		static void scrollCallback(GLFWwindow* pWindow, double pX, double pY);
		static void keyCallback(GLFWwindow* pWindow, int pKey, int pScancode, int pAction, int pMods);
		static EngineCore::EventSystem::Key getKey(int pKey);

		Window();
		Window(const char* pTitle, const unsigned int& pWidth, const unsigned int& pHeight);
		~Window();

		void close();
		void shutDown();
		bool shouldClose();
		void pollEvents();
		void waitEvent();
		void setWindowUserPointer(void* pPointer);
		int getKey(const unsigned int& pKey);
		void bindContext(bool pBinded);
		void setInputManager(EngineCore::EventSystem::InputManager* pInput);

		static const char** getRequiredInstanceExtensions(uint32_t& pExtensionCount);
		void getFramebufferSize(int* pWidth, int* pHeight);
		void updateFramebufferSize();

		HWND getWindowId();

		void changeCursorMode(bool pShowHide, bool pSaveState = true);
		void setMousePosition(Maths::FVector2& pPos);
		bool IsActiveWindow();
		
		template<typename T, typename K> int createWindowSurface(void* pInstance, void* pSurface)
		{
			if (pInstance == nullptr)
				return -1;
			return glfwCreateWindowSurface((T)pInstance, mWindow, nullptr, (K)pSurface);
		}
	};
}