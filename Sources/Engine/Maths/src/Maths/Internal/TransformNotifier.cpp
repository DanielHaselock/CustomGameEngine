#include "Maths/Internal/TransformNotifier.h"

using namespace Maths::Internal;

TransformNotifier::NotificationHandlerID TransformNotifier::addNotificationHandler(NotificationHandler pNotificationHandler)
{
	NotificationHandlerID handlerID = m_availableHandlerID++;
	m_notificationHandlers.emplace(handlerID, pNotificationHandler);
	return handlerID;
}

void TransformNotifier::notifyChildren(ENotification pNotification)
{
	if (!m_notificationHandlers.empty())
		for (auto const&[id, handler] : m_notificationHandlers)
			handler(pNotification);
}

bool TransformNotifier::removeNotificationHandler(const NotificationHandlerID& pNotificationHandlerID)
{
	return m_notificationHandlers.erase(pNotificationHandlerID) != 0;
}