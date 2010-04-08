/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/StreamStack/PlatformTLSLayerFactory.h"

#include <cassert>

#ifdef HAVE_OPENSSL
#include "Swiften/StreamStack/OpenSSLLayer.h"
#endif

namespace Swift {

PlatformTLSLayerFactory::PlatformTLSLayerFactory() {
}

bool PlatformTLSLayerFactory::canCreate() const {
#ifdef HAVE_OPENSSL
	return true;
#else
	return false;
#endif
}

boost::shared_ptr<TLSLayer> PlatformTLSLayerFactory::createTLSLayer() {
#ifdef HAVE_OPENSSL
	return boost::shared_ptr<TLSLayer>(new OpenSSLLayer());
#else
	assert(false);
	return boost::shared_ptr<TLSLayer>();
#endif
}

}
