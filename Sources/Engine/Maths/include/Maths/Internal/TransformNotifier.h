#pragma once

#include <functional>


namespace Maths::Internal
{
	class TransformNotifier
	{
	public:
		enum class ENotification
		{
			TRANSFORM_CHANGED,
			TRANSFORM_DESTROYED
		};

		using NotificationHandler = std::function<void(ENotification)>;

		using NotificationHandlerID = uint64_t;

		NotificationHandlerID addNotificationHandler(NotificationHandler pNotificationHandler);

		void notifyChildren(ENotification pNotification);

		bool removeNotificationHandler(const NotificationHandlerID& pNotificationHandlerID);

		std::unordered_map<NotificationHandlerID, NotificationHandler> m_notificationHandlers;
		NotificationHandlerID m_availableHandlerID = 0;
	};
}