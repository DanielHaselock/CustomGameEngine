#include <QtCore/qdir.h>
#include <QtCore/qdiriterator.h>

#include <QtWidgets/qmessagebox.h>
#include "EngineCore/Service/ServiceLocator.h"
#include "EngineCore/EventSystem/InputManager.h"
#include <Tools/Utils/PathParser.h>
#include <Tools/Utils/Define.h>
#include "Editor/Utils/Utils.h"

using namespace Editor;

QString Utils::uniqueString(const QString& pBase, const char* pExtension)
{
	int idx = -1;
	
	QString path;
	do
	{
		idx++;
		path = pBase + (idx > 0 ? "(" + QString::number(idx) + ")" : "") + "." + pExtension;
	} while (QFile::exists(path));
	
	return path;
}

void Utils::copyAndReplaceFolderContents(const QString& pFromDir, const QString& pToDir, bool pCopyAndRemove, QString pExeptFile)
{
    QDirIterator it(pFromDir, QDirIterator::Subdirectories);
    QDir dir(pFromDir);
    const int absSourcePathLength = dir.absoluteFilePath(pFromDir).length();

    while (it.hasNext())
    {
        it.next();
        const auto fileInfo = it.fileInfo();
        if (!fileInfo.isHidden())
        {
            const QString subPathStructure = fileInfo.absoluteFilePath().mid(absSourcePathLength);
            const QString constructedAbsolutePath = pToDir + subPathStructure;

			if (fileInfo.isDir())
				if (!pExeptFile.isEmpty() && fileInfo.absoluteFilePath().contains(pExeptFile))
					continue;
				else
					dir.mkpath(constructedAbsolutePath);
            else if (fileInfo.isFile())
            {
                QFile::remove(constructedAbsolutePath);
                QFile::copy(fileInfo.absoluteFilePath(), constructedAbsolutePath);
            }
        }
    }

    if (pCopyAndRemove)
        dir.removeRecursively();
}

void Utils::copyFileAndDir(const QString& pFromDir, const QString& pToDir, QWidget* pParent, bool pCopyAndRemove)
{
	if (pFromDir == pToDir)
		return;

	const QString fileName = pFromDir;
	const QString newFileName = fileName.split("/").last();
	const QString newFile = pToDir + "/" + newFileName;

	QFileInfo fileInfos(fileName);
	if (fileInfos.isDir())
	{
		QDirIterator it(fileName, QDirIterator::Subdirectories);
		QDir dir(fileName);
		const int absSourcePathLength = dir.absoluteFilePath(fileName).length();

		while (it.hasNext())
		{
			it.next();
			const QFileInfo fileInfo = it.fileInfo();
			if (!fileInfo.isHidden())
			{
				const QString subPathStructure = fileInfo.absoluteFilePath().mid(absSourcePathLength);
				const QString constructedAbsolutePath = newFile + subPathStructure;

				if (fileInfo.isDir())
					dir.mkpath(constructedAbsolutePath);
				else if (fileInfo.isFile())
				{
					if (QFile::exists(constructedAbsolutePath))
					{
						QMessageBox msgBox(QMessageBox::Question,
							"",
							QString("%1 already exists. \nDo you want to overwrite it?\n").arg(constructedAbsolutePath.split("/").last()),
							QMessageBox::Yes | QMessageBox::No, pParent,
							Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

						if (msgBox.exec() == QMessageBox::Yes)
						{
							QFile::remove(constructedAbsolutePath);
							QFile::copy(fileInfo.absoluteFilePath(), constructedAbsolutePath);
						}
					}
					else
						QFile::copy(fileInfo.absoluteFilePath(), constructedAbsolutePath);
				}
			}
		}

		if (pCopyAndRemove)
			dir.removeRecursively();
	}
	else
	{
		if (QFile::exists(newFile))
		{
			QMessageBox msgBox(QMessageBox::Question,
				"",
				QString("%1 already exists. \nDo you want to overwrite it?\n").arg(newFileName),
				QMessageBox::Yes | QMessageBox::No, pParent,
				Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

			if (msgBox.exec() == QMessageBox::Yes)
			{
				QFile::remove(newFile);
				QFile::copy(fileName, newFile);

				if (pCopyAndRemove)
					QFile::remove(fileName);
			}
		}
		else
		{
			QFile::copy(fileName, newFile);

			if (pCopyAndRemove)
				QFile::remove(fileName);
		}
	}
}

std::string Utils::qStringToStdString(const QString& pString)
{
    return pString.toLocal8Bit().constData();
}

void Utils::sendQtInput(QKeyEvent* pKeyEvent, EngineCore::EventSystem::KState pState)
{
	EngineCore::EventSystem::Key key = EngineCore::EventSystem::Key::UNKNOWN;
	
	if (pKeyEvent->key() == Qt::Key_A)
		key = EngineCore::EventSystem::Key::A;
	else if (pKeyEvent->key() == Qt::Key_B)
		key = EngineCore::EventSystem::Key::B;
	else if (pKeyEvent->key() == Qt::Key_C)
		key = EngineCore::EventSystem::Key::C;
	else if (pKeyEvent->key() == Qt::Key_D)
		key = EngineCore::EventSystem::Key::D;
	else if (pKeyEvent->key() == Qt::Key_E)
		key = EngineCore::EventSystem::Key::E;
	else if (pKeyEvent->key() == Qt::Key_F)
		key = EngineCore::EventSystem::Key::F;
	else if (pKeyEvent->key() == Qt::Key_G)
		key = EngineCore::EventSystem::Key::G;
	else if (pKeyEvent->key() == Qt::Key_H)
		key = EngineCore::EventSystem::Key::H;
	else if (pKeyEvent->key() == Qt::Key_I)
		key = EngineCore::EventSystem::Key::I;
	else if (pKeyEvent->key() == Qt::Key_J)
		key = EngineCore::EventSystem::Key::J;
	else if (pKeyEvent->key() == Qt::Key_K)
		key = EngineCore::EventSystem::Key::K;
	else if (pKeyEvent->key() == Qt::Key_L)
		key = EngineCore::EventSystem::Key::L;
	else if (pKeyEvent->key() == Qt::Key_M)
		key = EngineCore::EventSystem::Key::M;
	else if (pKeyEvent->key() == Qt::Key_N)
		key = EngineCore::EventSystem::Key::N;
	else if (pKeyEvent->key() == Qt::Key_O)
		key = EngineCore::EventSystem::Key::O;
	else if (pKeyEvent->key() == Qt::Key_P)
		key = EngineCore::EventSystem::Key::P;
	else if (pKeyEvent->key() == Qt::Key_Q)
		key = EngineCore::EventSystem::Key::Q;
	else if (pKeyEvent->key() == Qt::Key_R)
		key = EngineCore::EventSystem::Key::R;
	else if (pKeyEvent->key() == Qt::Key_S)
		key = EngineCore::EventSystem::Key::S;
	else if (pKeyEvent->key() == Qt::Key_T)
		key = EngineCore::EventSystem::Key::T;
	else if (pKeyEvent->key() == Qt::Key_U)
		key = EngineCore::EventSystem::Key::U;
	else if (pKeyEvent->key() == Qt::Key_V)
		key = EngineCore::EventSystem::Key::V;
	else if (pKeyEvent->key() == Qt::Key_W)
		key = EngineCore::EventSystem::Key::W;
	else if (pKeyEvent->key() == Qt::Key_X)
		key = EngineCore::EventSystem::Key::X;
	else if (pKeyEvent->key() == Qt::Key_Y)
		key = EngineCore::EventSystem::Key::Y;
	else if (pKeyEvent->key() == Qt::Key_Z)
		key = EngineCore::EventSystem::Key::Z;
	else if (pKeyEvent->key() == Qt::Key_Escape)
		key = EngineCore::EventSystem::Key::ESCAPE;
	else if (pKeyEvent->key() == Qt::Key_1)
		key = EngineCore::EventSystem::Key::_1;
	else if (pKeyEvent->key() == Qt::Key_2)
		key = EngineCore::EventSystem::Key::_2;
	else if (pKeyEvent->key() == Qt::Key_3)
		key = EngineCore::EventSystem::Key::_3;
	else if (pKeyEvent->key() == Qt::Key_4)
		key = EngineCore::EventSystem::Key::_4;
	else if (pKeyEvent->key() == Qt::Key_5)
		key = EngineCore::EventSystem::Key::_5;
	else if (pKeyEvent->key() == Qt::Key_6)
		key = EngineCore::EventSystem::Key::_6;
	else if (pKeyEvent->key() == Qt::Key_7)
		key = EngineCore::EventSystem::Key::_7;
	else if (pKeyEvent->key() == Qt::Key_8)
		key = EngineCore::EventSystem::Key::_8;
	else if (pKeyEvent->key() == Qt::Key_9)
		key = EngineCore::EventSystem::Key::_9;
	else if (pKeyEvent->key() == Qt::Key_Tab)
		key = EngineCore::EventSystem::Key::TAB;
	else if (pKeyEvent->key() == Qt::Key_Enter)
		key = EngineCore::EventSystem::Key::ENTER;
	else if (pKeyEvent->key() == Qt::Key_Shift)
		key = EngineCore::EventSystem::Key::LEFT_SHIFT;
	else if (pKeyEvent->key() == Qt::Key_Control)
		key = EngineCore::EventSystem::Key::LEFT_CTRL;
	else if (pKeyEvent->key() == Qt::Key_Space)
		key = EngineCore::EventSystem::Key::SPACE;

	if (key == EngineCore::EventSystem::Key::UNKNOWN)
		return;

	service(EngineCore::EventSystem::InputManager).receiveInput(pState, key);
}

Tools::Utils::PathParser::EFileType Utils::getFileType(const QString& pExtension)
{
	std::string pExt = Utils::qStringToStdString(pExtension);
	std::transform(pExt.begin(), pExt.end(), pExt.begin(), ::tolower);

	if (pExt == "fbx" || pExt == "obj" || pExt == "dae" || pExt == "gltf")
		return Tools::Utils::PathParser::EFileType::MODEL;
	else if (pExt == "png" || pExt == "jpeg" || pExt == "jpg" || pExt == "bmp")
		return Tools::Utils::PathParser::EFileType::TEXTURE;
	else if (pExt == Define::MATERIAL_EXTENSION)
		return Tools::Utils::PathParser::EFileType::MATERIAL;
	else if (pExt == "wav" || pExt == "mp3" || pExt == "ogg")
		return Tools::Utils::PathParser::EFileType::SOUND;
	else if (pExt == Define::SCENE_EXTENSION)
		return Tools::Utils::PathParser::EFileType::SCENE;
	else if (pExt == Define::CLASS_EXTENSION)
		return Tools::Utils::PathParser::EFileType::SCRIPT;
	else if (pExt == "ttf")
		return Tools::Utils::PathParser::EFileType::FONT;
	else if (pExt == "ux")
		return Tools::Utils::PathParser::EFileType::UI;
	else if (pExt == "fab")
		return Tools::Utils::PathParser::EFileType::PREFAB;
	else if (pExt == "pat")
		return Tools::Utils::PathParser::EFileType::PARTICLE;

	return Tools::Utils::PathParser::EFileType::UNKNOWN;
}