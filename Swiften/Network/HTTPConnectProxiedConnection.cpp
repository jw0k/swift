/*
 * Copyright (c) 2010-2011 Thilo Cestonaro
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2011 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */


#include <Swiften/Network/HTTPConnectProxiedConnection.h>

#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>

#include <Swiften/Base/Algorithm.h>
#include <Swiften/Base/Log.h>
#include <Swiften/Base/String.h>
#include <Swiften/Base/ByteArray.h>
#include <Swiften/Network/HostAddressPort.h>
#include <Swiften/Network/ConnectionFactory.h>
#include <Swiften/StringCodecs/Base64.h>

using namespace Swift;

HTTPConnectProxiedConnection::HTTPConnectProxiedConnection(ConnectionFactory* connectionFactory, HostAddressPort proxy, const SafeString& authID, const SafeString& authPassword) : connectionFactory_(connectionFactory), proxy_(proxy), server_(HostAddressPort(HostAddress("0.0.0.0"), 0)), authID_(authID), authPassword_(authPassword) {
	connected_ = false;
}

HTTPConnectProxiedConnection::~HTTPConnectProxiedConnection() {
	if (connection_) {
		connection_->onConnectFinished.disconnect(boost::bind(&HTTPConnectProxiedConnection::handleConnectionConnectFinished, shared_from_this(), _1));
		connection_->onDataRead.disconnect(boost::bind(&HTTPConnectProxiedConnection::handleDataRead, shared_from_this(), _1));
		connection_->onDisconnected.disconnect(boost::bind(&HTTPConnectProxiedConnection::handleDisconnected, shared_from_this(), _1));
	}

	if (connected_) {
		std::cerr << "Warning: Connection was still established." << std::endl;
	}
}

void HTTPConnectProxiedConnection::connect(const HostAddressPort& server) {
	server_ = server;
	connection_ = connectionFactory_->createConnection();
	connection_->onConnectFinished.connect(boost::bind(&HTTPConnectProxiedConnection::handleConnectionConnectFinished, shared_from_this(), _1));
	connection_->onDataRead.connect(boost::bind(&HTTPConnectProxiedConnection::handleDataRead, shared_from_this(), _1));
	connection_->onDisconnected.connect(boost::bind(&HTTPConnectProxiedConnection::handleDisconnected, shared_from_this(), _1));
	connection_->connect(proxy_);
}

void HTTPConnectProxiedConnection::listen() {
	assert(false);
	connection_->listen();
}

void HTTPConnectProxiedConnection::disconnect() {
	connected_ = false;
	connection_->disconnect();
}

void HTTPConnectProxiedConnection::handleDisconnected(const boost::optional<Error>& error) {
	onDisconnected(error);
}

void HTTPConnectProxiedConnection::write(const SafeByteArray& data) {
	connection_->write(data);
}

void HTTPConnectProxiedConnection::handleConnectionConnectFinished(bool error) {
	connection_->onConnectFinished.disconnect(boost::bind(&HTTPConnectProxiedConnection::handleConnectionConnectFinished, shared_from_this(), _1));
	if (!error) {
		std::stringstream connect;
		connect << "CONNECT " << server_.getAddress().toString() << ":" << server_.getPort() << " HTTP/1.1\r\n";
		SafeByteArray data = createSafeByteArray(connect.str());
		if (!authID_.empty() && !authPassword_.empty()) {
			append(data, createSafeByteArray("Proxy-Authorization: Basic "));
			SafeByteArray credentials = authID_;
			append(credentials, createSafeByteArray(":"));
			append(credentials, authPassword_);
			append(data, Base64::encode(credentials));
			append(data, createSafeByteArray("\r\n"));
		}
		append(data, createSafeByteArray("\r\n"));
		SWIFT_LOG(debug) << "HTTP Proxy send headers: " << byteArrayToString(ByteArray(data.begin(), data.end())) << std::endl;
		connection_->write(data);
	}
	else {
		onConnectFinished(true);
	}
}

void HTTPConnectProxiedConnection::handleDataRead(boost::shared_ptr<SafeByteArray> data) {
	if (!connected_) {
		SWIFT_LOG(debug) << byteArrayToString(ByteArray(data->begin(), data->end())) << std::endl;
		std::vector<std::string> tmp = String::split(byteArrayToString(ByteArray(data->begin(), data->end())), ' ');
		if(tmp.size() > 1) {
			int status = boost::lexical_cast<int> (tmp[1].c_str()); 
			SWIFT_LOG(debug) << "Proxy Status: " << status << std::endl;
			if (status / 100 == 2) { // all 2XX states are OK
				connected_ = true;
				onConnectFinished(false);
				return;
			}
			SWIFT_LOG(debug) << "HTTP Proxy returned an error: " << byteArrayToString(ByteArray(data->begin(), data->end())) << std::endl;
		}
		disconnect();
		onConnectFinished(true);
	}
	else {
		onDataRead(data);
	}
}

HostAddressPort HTTPConnectProxiedConnection::getLocalAddress() const {
	return connection_->getLocalAddress();
}
