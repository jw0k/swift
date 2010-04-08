/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/RosterSerializer.h"

#include <boost/shared_ptr.hpp>

#include "Swiften/Base/foreach.h"
#include "Swiften/Serializer/XML/XMLTextNode.h"
#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {

RosterSerializer::RosterSerializer() : GenericPayloadSerializer<RosterPayload>() {
}

String RosterSerializer::serializePayload(boost::shared_ptr<RosterPayload> roster)  const {
	XMLElement queryElement("query", "jabber:iq:roster");
	foreach(const RosterItemPayload& item, roster->getItems()) {
		boost::shared_ptr<XMLElement> itemElement(new XMLElement("item"));
		itemElement->setAttribute("jid", item.getJID());
		itemElement->setAttribute("name", item.getName());

		switch (item.getSubscription()) {
			case RosterItemPayload::To: itemElement->setAttribute("subscription", "to"); break;
			case RosterItemPayload::From: itemElement->setAttribute("subscription", "from"); break;
			case RosterItemPayload::Both: itemElement->setAttribute("subscription", "both"); break;
			case RosterItemPayload::Remove: itemElement->setAttribute("subscription", "remove"); break;
			case RosterItemPayload::None: itemElement->setAttribute("subscription", "none"); break;
		}

		if (item.getSubscriptionRequested()) {
			itemElement->setAttribute("ask", "subscribe");
		}

		foreach(const String& group, item.getGroups()) {
			boost::shared_ptr<XMLElement> groupElement(new XMLElement("group"));
			groupElement->addNode(boost::shared_ptr<XMLTextNode>(new XMLTextNode(group)));
			itemElement->addNode(groupElement);
		}

		queryElement.addNode(itemElement);
	}

	return queryElement.serialize();
}

}
