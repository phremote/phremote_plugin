#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <random>
#include "cpprest/json.h"
#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "cpprest/asyncrt_utils.h"
#include "cpprest/json.h"
#include "cpprest/filestream.h"
#include "cpprest/containerstream.h"
#include "cpprest/producerconsumerstream.h"


using namespace std;
using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;



class handler
{
public:
	handler();
	handler(utility::string_t url);
	virtual ~handler();

	pplx::task<void>open() { return m_listener.open(); }
	pplx::task<void>close() { return m_listener.close(); }

protected:

private:
	void handle_get(http_request message);
	void handle_error(pplx::task<void>& t);
	http_listener m_listener;
};



handler::handler()
{
	//ctor
}
handler::handler(utility::string_t url) :m_listener(url)
{
	m_listener.support(methods::GET, std::bind(&handler::handle_get, this, std::placeholders::_1));
}
handler::~handler()
{
	//dtor
}

void handler::handle_error(pplx::task<void>& t)
{
	try
	{
		t.get();
	}
	catch (...)
	{
		// Ignore the error
	}
}


int max_cpu;

//
// Get Request 
//
void handler::handle_get(http_request message)
{

	auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));

	message.relative_uri().path();

	wchar_t buf[10];
	swprintf(buf, 10, L"max cpu %d", max_cpu);

	message.reply(status_codes::OK, buf);
	return;
};


std::unique_ptr<handler> g_httpHandler;

void on_initialize(const string_t& address)
{


	uri_builder uri(address);


	auto addr = uri.to_uri().to_string();
	g_httpHandler = std::unique_ptr<handler>(new handler(addr));
	g_httpHandler->open().wait();

	return;
}

void start_server()
{
	utility::string_t port = U("1234");

	utility::string_t address = U("http://127.0.0.1:");
	address.append(port);

	on_initialize(address);

}




extern "C" {
	void StartRESTService() {
		start_server();
	};
	void UpdateMaxCPU(
		int new_mcpu
	)
	{
		max_cpu = new_mcpu;
	};
}


