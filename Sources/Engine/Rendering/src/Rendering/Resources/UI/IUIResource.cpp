#include "Rendering/Resources/UI/IUIResource.h"
#include "Game/Component/CPCamera.h"

#ifdef NSHIPPING
#include "Editor/Widget/WidgetEditor.h"
#else
#include "EngineCore/Core/EngineApp.h"
#endif

using namespace Rendering::Resources::UIResource;

bool Rect::contain(float x, float y)
{
	return contain({ x, y });
}

bool Rect::contain(Maths::FVector2 p)
{
	return p.x >= corner.x && p.y >= corner.y && p.x <= corner.x + size.x && p.y <= corner.y + size.y;
}

IUIResource::IUIResource(Rect pRect, IUI pType) : position(pRect), mType(pType) 
{
	int a = 0;
}

IUIResource::IUIResource(const IUIResource& pUI)
{
	position = pUI.position;
	mType = pUI.mType;
}

bool IUIResource::contain(float x, float y)
{
	if (isHidden)
		return false;

	return contain({ x, y });
}

bool IUIResource::contain(Maths::FVector2 p)
{
	if (isHidden)
		return false;

	return position.contain(p);
}

void IUIResource::CallLuaDelegatePress()
{ 
	if (mLuaFunctionPress)
	{
#ifdef NSHIPPING
		try {
			mLuaFunctionPress();
		}
		catch (sol::error err)
		{
			Editor::Widget::WidgetConsole::errorPrint("%s", err.what());
		}
#else
		mLuaFunctionPress();
#endif
	}
}

void IUIResource::setAllColor(Maths::FVector4 pNewColor)
{
	current->color = pNewColor;

	setNormalColor(pNewColor);
	setHoverColor(pNewColor);
	setPressColor(pNewColor);
}

void IUIResource::setNormalColor(Maths::FVector4 pNewColor)
{
	normal.color = pNewColor;
}

void IUIResource::setHoverColor(Maths::FVector4 pNewColor)
{
	hover.color = pNewColor;
}

void IUIResource::setPressColor(Maths::FVector4 pNewColor)
{
	press.color = pNewColor;
}

void Rendering::Resources::UIResource::IUIResource::setVisible(bool pIsVisible)
{
	isHidden = !pIsVisible;
}

void Rendering::Resources::UIResource::IUIResource::setScale(Maths::FVector2 pNewScale)
{
	EngineCore::Core::EngineApp& app = service(EngineCore::Core::EngineApp);

	position.ratio = pNewScale;
	updateData(app.uiProj, app.mWidth, app.mHeight);
}

Maths::FVector2 Rendering::Resources::UIResource::IUIResource::getScale()
{
	return position.ratio;
}
