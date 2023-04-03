#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/beast.hpp>
#include <iomanip>
#include <iostream>
#include <string>

using tcp = boost::asio::ip::tcp;


namespace net = boost::asio;
namespace beast = boost::beast;
using namespace boost::beast;
using namespace boost::beast::websocket;




void Log(const std::string& where, boost::system::error_code ec)
{
    std::cerr << "[" << std::setw(10) << where << std::setw(5) << "] "
              << (ec ? "Error: " : "OK")
              << (ec ? ec.message() : "") 
              << std::endl;
}

int main()
{

    const std::string host = {"ltnm.learncppthroughprojects.com"};
    const std::string port {"80"};
    const std::string message {"hello learncppthroughprojects :)"};

    net::io_context ioc {};

    tcp::resolver resolver {ioc};

    boost::system::error_code ec {};
    auto resolverIt {resolver.resolve(host, port, ec)};
    if (ec) {
        Log("resolver.resolve", ec);
        return -1;
    }
    tcp::socket socket {ioc};

    socket.connect(*resolverIt, ec);
    if (ec) {
        Log("socket.connect", ec);
        return -2;
    }
    websocket::stream<boost::beast::tcp_stream> ws{std::move(socket)};

    ws.handshake(host, "/echo",ec);
    if (ec) {
        Log("ws.handshake", ec);
        return -3;
    }

    ws.text(true);
    
    
    boost::asio::const_buffer wbuffer {message.c_str(), message.size()};
    ws.write(wbuffer);
    if (ec) {
        Log("ws.write", ec);
        return -4;
    }

    beast::flat_buffer rbuffer {};

    ws.read(rbuffer, ec);
    if (ec) {
        Log("ws.read", ec);
        return 51;
    }

    // Print the echoed message.
    std::cout << "ECHO: "
          << boost::beast::make_printable(rbuffer.data())
          << std::endl;

    Log("returning", ec);
    return 0;
}