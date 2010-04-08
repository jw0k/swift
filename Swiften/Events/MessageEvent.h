/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_MessageEvent_H
#define SWIFTEN_MessageEvent_H

#include <cassert>

#include <boost/signals.hpp>
#include <boost/shared_ptr.hpp>

#include "Swiften/Events/StanzaEvent.h"
#include "Swiften/Elements/Message.h"

namespace Swift {
	class MessageEvent : public StanzaEvent {
		public:
			MessageEvent(boost::shared_ptr<Message> stanza) : stanza_(stanza){};
			virtual ~MessageEvent(){};
			boost::shared_ptr<Message> getStanza() {return stanza_;}

			bool isReadable() {
				return getStanza()->isError() || !getStanza()->getBody().isEmpty();
			}

			void read() {
				assert (isReadable());
				conclude();
			}

		private:
			boost::shared_ptr<Message> stanza_;
	};
}

#endif
