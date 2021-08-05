#pragma once

#include <Asio.hpp>
#include <string>
#include <functional>
#include <boost/lexical_cast.hpp>

enum class fct_lookup_protocol {V4, V6, ANY};

template <typename InternetProtocol>
void fct_async_resolve(std::string host, uint16_t port, typename InternetProtocol::resolver &resolver, std::function<void(const boost::system::error_code&, typename InternetProtocol::resolver::iterator)> handleResolve) {
    fct_async_resolve<InternetProtocol>(host, port, resolver, handleResolve, fct_lookup_protocol::ANY);
}

/**
 * This was originally a wrapper around resolver::async_resolve which first attempted to interpret the hostname as a literal IP address
 * and it could be parsed this way, the handler function will be called synchronously.
 * Unfortunately this iterator pattern is deprecated by boost and I can't figure out how to rewrite it so I took that bit out. Sorry. -cosmonaut
 */
template <typename InternetProtocol>
void fct_async_resolve(std::string host, uint16_t port, typename InternetProtocol::resolver &resolver, std::function<void(const boost::system::error_code&, typename InternetProtocol::resolver::iterator)> handleResolve, fct_lookup_protocol protocol) {
	boost::system::error_code ec;
	typename InternetProtocol::endpoint endpoint(boost::asio::ip::address::from_string(host, ec), port);

	if(!ec) {
        handleResolve(ec, typename InternetProtocol::resolver::iterator());
	} else {
	    std::string portStr(boost::lexical_cast<std::string>(port));
	    typename InternetProtocol::resolver::query::flags flags(InternetProtocol::resolver::query::numeric_service | InternetProtocol::resolver::query::address_configured);
	    typename InternetProtocol::resolver::query query([&](){
	        if(protocol == fct_lookup_protocol::ANY) {
                return typename InternetProtocol::resolver::query(host, portStr, flags);
	        } else {
	            return typename InternetProtocol::resolver::query((protocol == fct_lookup_protocol::V4) ? InternetProtocol::v4() : InternetProtocol::v6(), host, portStr, flags);
	        }
	    }());

        resolver.async_resolve(query, handleResolve);
	}
}
