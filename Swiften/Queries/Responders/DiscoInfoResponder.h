/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#ifndef SWIFTEN_DiscoInfoResponder_H
#define SWIFTEN_DiscoInfoResponder_H

#include <map>

#include "Swiften/Queries/GetResponder.h"
#include "Swiften/Elements/DiscoInfo.h"

namespace Swift {
	class IQRouter;

	class DiscoInfoResponder : public GetResponder<DiscoInfo> {
		public:
			DiscoInfoResponder(IQRouter* router);

			void setDiscoInfo(const DiscoInfo& info);
			void setDiscoInfo(const String& node, const DiscoInfo& info);

		private:
			virtual bool handleGetRequest(const JID& from, const String& id, boost::shared_ptr<DiscoInfo> payload);

		private:
			DiscoInfo info_;
			std::map<String, DiscoInfo> nodeInfo_;
	};
}

#endif
