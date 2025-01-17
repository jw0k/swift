/*
 * Copyright (c) 2010-2019 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Network/BoostNetworkFactories.h>

#include <Swiften/Crypto/CryptoProvider.h>
#include <Swiften/Crypto/PlatformCryptoProvider.h>
#include <Swiften/IDN/IDNConverter.h>
#include <Swiften/IDN/PlatformIDNConverter.h>
#include <Swiften/Network/BoostConnectionFactory.h>
#include <Swiften/Network/BoostConnectionServerFactory.h>
#include <Swiften/Network/BoostTimerFactory.h>
#include <Swiften/Network/NullNATTraverser.h>
#include <Swiften/Network/PlatformNATTraversalWorker.h>
#include <Swiften/Network/PlatformNetworkEnvironment.h>
#include <Swiften/Network/PlatformProxyProvider.h>
#include <Swiften/Network/TLSConnectionFactory.h>
#include <Swiften/Parser/PlatformXMLParserFactory.h>
#include <Swiften/TLS/PlatformTLSFactories.h>

#ifdef USE_UNBOUND
#include <Swiften/Network/UnboundDomainNameResolver.h>
#else
#include <Swiften/Network/PlatformDomainNameResolver.h>
#endif

namespace Swift {

BoostNetworkFactories::BoostNetworkFactories(EventLoop* eventLoop, std::shared_ptr<boost::asio::io_service> ioService, bool useOpportunisticTLS , TLSOptions tlsOptions) : ioServiceThread(ioService), eventLoop(eventLoop) {
    timerFactory = new BoostTimerFactory(ioServiceThread.getIOService(), eventLoop);

    tlsFactories = new PlatformTLSFactories();
    if (useOpportunisticTLS)
    {
        boostConnectionFactory = nullptr;
        connectionFactory = new BoostConnectionFactory(ioServiceThread.getIOService(), eventLoop);
    }
    else
    {
        boostConnectionFactory = new BoostConnectionFactory(ioServiceThread.getIOService(), eventLoop);
        connectionFactory = new TLSConnectionFactory(tlsFactories->getTLSContextFactory(), boostConnectionFactory, tlsOptions);
    }

    connectionServerFactory = new BoostConnectionServerFactory(ioServiceThread.getIOService(), eventLoop);
#ifdef SWIFT_EXPERIMENTAL_FT
    natTraverser = new PlatformNATTraversalWorker(eventLoop);
#else
    natTraverser = new NullNATTraverser(eventLoop);
#endif
    networkEnvironment = new PlatformNetworkEnvironment();
    xmlParserFactory = new PlatformXMLParserFactory();
    proxyProvider = new PlatformProxyProvider();
    idnConverter = PlatformIDNConverter::create();
#ifdef USE_UNBOUND
    // TODO: What to do about idnConverter.
    domainNameResolver = new UnboundDomainNameResolver(idnConverter.get(), ioServiceThread.getIOService(), eventLoop);
#else
    domainNameResolver = new PlatformDomainNameResolver(idnConverter.get(), eventLoop);
#endif
    cryptoProvider = PlatformCryptoProvider::create();
}

BoostNetworkFactories::~BoostNetworkFactories() {
    delete cryptoProvider;
    delete domainNameResolver;
    delete proxyProvider;
    delete tlsFactories;
    delete xmlParserFactory;
    delete networkEnvironment;
    delete natTraverser;
    delete connectionServerFactory;
    delete connectionFactory;
    delete timerFactory;
    delete boostConnectionFactory;
}

TLSContextFactory* BoostNetworkFactories::getTLSContextFactory() const {
    return tlsFactories->getTLSContextFactory();
}

}
