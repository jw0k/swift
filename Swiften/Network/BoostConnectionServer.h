/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/signal.hpp>

#include "Swiften/Network/BoostConnection.h"
#include "Swiften/Network/ConnectionServer.h"
#include "Swiften/EventLoop/EventOwner.h"

namespace Swift {
	class BoostConnectionServer : public ConnectionServer, public EventOwner, public boost::enable_shared_from_this<BoostConnectionServer> {
		public:
			enum Error {
				Conflict,
				UnknownError
			};
			BoostConnectionServer(int port, boost::asio::io_service* ioService);

			void start();
			void stop();

			boost::signal<void (boost::optional<Error>)> onStopped;

		private:
			void stop(boost::optional<Error> e);
			void acceptNextConnection();
			void handleAccept(boost::shared_ptr<BoostConnection> newConnection, const boost::system::error_code& error);

		private:
			int port_;
			boost::asio::io_service* ioService_;
			boost::asio::ip::tcp::acceptor* acceptor_;
	};
}
