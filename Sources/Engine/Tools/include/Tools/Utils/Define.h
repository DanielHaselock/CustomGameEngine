#pragma once

class Define
{
	public:

		Define() = delete;

		static const char* PROJECT_EXTENSION;
		static const char* SCENE_EXTENSION;
		static const char* CLASS_EXTENSION;
		static const char* MATERIAL_EXTENSION;
		static const char* UI_EXTENSION;
		static const char* PREFAB_EXTENSION;
		static const char* PARTICLE_EXTENSION;

		enum EEditorState
		{
			CLOSE_EDITOR,
			OPEN_EDITOR,
			OPEN_HUB,
			LOAD_EDITOR
		};
};