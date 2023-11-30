#pragma once
#include <Editor/UI/UIResource.h>

using namespace Editor::UI;

QJsonObject UIResource::save() const
{
	QJsonObject modelJson = NodeDelegateModel::save();

	modelJson["hover"] = saveUIStyle(&hover);
	modelJson["press"] = saveUIStyle(&press);
	modelJson["normal"] = saveUIStyle(&normal);

	modelJson["isHidden"] = isHidden;
	modelJson["zOrder"] = zOrder;

	modelJson["name"] = mName;

	return modelJson;
}

void UIResource::load(QJsonObject const& pData)
{
	QJsonValue h = pData["hover"].toObject();
	if (!h.isUndefined())
		loadStyle(h.toObject(), hover);

	QJsonValue p = pData["press"].toObject();
	if (!p.isUndefined())
		loadStyle(p.toObject(), press);

	QJsonValue n = pData["normal"].toObject();
	if (!n.isUndefined())
		loadStyle(n.toObject(), normal);

	isHidden = pData["isHidden"].toBool();
	zOrder = pData["zOrder"].toInt();
	mName = pData["name"].toString();
}

QJsonObject UIResource::saveUIStyle(const UIStyle* pStyle) const
{
	QJsonObject objJson;

	QJsonArray nodesJsonArray;
	nodesJsonArray.append(pStyle->color.red());
	nodesJsonArray.append(pStyle->color.green());
	nodesJsonArray.append(pStyle->color.blue());
	nodesJsonArray.append(pStyle->color.alpha());

	objJson["color"] = nodesJsonArray;

	QString finalStr = pStyle->pTextPath.isEmpty() ?
		"" :
		pStyle->pTextPath.split(service(Editor::Widget::WidgetEditor).mLocation.mFolder + "/")[1];

	objJson["texture"] = Utils::qStringToStdString(finalStr).c_str();;

	return objJson;
};

void UIResource::loadStyle(QJsonObject& jsonStyle, UIStyle& uiStyle)
{
	if (!jsonStyle["color"].isUndefined())
	{
		QJsonArray color = jsonStyle["color"].toArray();
		uiStyle.color = QColor(color[0].toInt(), color[1].toInt(), color[2].toInt(), color[3].toInt());
	}

	QJsonValue v = jsonStyle["texture"];

	if (!v.isUndefined())
	{
		if (v.toString().isEmpty())
			return;
				
		QString path = service(Editor::Widget::WidgetEditor).mLocation.mFolder + "/" + v.toString();
		uiStyle.pTextPath = path;
	}
}