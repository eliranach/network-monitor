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



void Log(const std::string& where, boost::system::error_code& ec)
{
    std::cerr << "[" << std::setw(10) << where << std::setw(5) << "] "
              << (ec ? "Error: " : "OK")
              << (ec ? ec.message() : "") 
              << std::endl;
}


void OnReceive(boost::beast::flat_buffer& rBuffer, boost::system::error_code& ec){
    if (ec) {
        Log("OnReceive", ec);
        return;
    }
        std::cout << "ECHO: "
              << boost::beast::make_printable(rBuffer.data())
              << std::endl;

}


void OnSend( websocket::stream<boost::beast::tcp_stream>& ws,
    boost::beast::flat_buffer& rBuffer, boost::system::error_code& ec){

        int i {0};
        if (ec) {
        Log("OnSend", ec);
        return;
    }
    // Read the echoed message back.
    ws.async_read(rBuffer,
        [&rBuffer](auto ec, auto nBytesRead) {
            OnReceive(rBuffer, ec);
        }
    );
     
}


void OnHandshake(    // --> Start of shared data  
    websocket::stream<boost::beast::tcp_stream>& ws,
    const boost::asio::const_buffer& wBuffer,
    boost::beast::flat_buffer& rBuffer,
    // <-- End of shared data
    boost::system::error_code& ec){
    if (ec){
        Log("OnHandshake", ec);
        return;
    }
     ws.text(true);
     ws.async_write(wBuffer, [&rBuffer, &ws] (auto ec, auto nBytesWritten){
            OnSend(ws, rBuffer, ec);
     });
    

}

void OnConnect(boost::system::error_code& ec,  
               websocket::stream<boost::beast::tcp_stream>& ws,
                const std::string& host,  
                 const std::string& port,
                 const std::string& message,
                const boost::asio::const_buffer& wBuffer,
                boost::beast::flat_buffer& rBuffer
    ){
    if (ec){
        Log("OnConnect", ec);
        return;
    }
    
    const std::string endpoint {"/echo"};

    ws.async_handshake(host, endpoint, [&ws, &wBuffer, &rBuffer](auto ec){
        OnHandshake(ws, wBuffer, rBuffer, ec);
    });
}



void OnResolve(boost::system::error_code& ec,  
                tcp::resolver::iterator resolverIt,
                 const std::string& host,  
                 const std::string& port,
                 const std::string& message,
                 websocket::stream<boost::beast::tcp_stream>& ws,
                const boost::asio::const_buffer& wBuffer,
                boost::beast::flat_buffer& rBuffer
              ){
    if (ec){
        Log("OnResolve", ec);
        return;
    }
    ws.next_layer().async_connect(*resolverIt,
        [&host, &port, &message, &ws,&wBuffer, &rBuffer](auto ec) {
            OnConnect(ec, ws, host,port, message, wBuffer, rBuffer);
        }
    );
}




int main()
{

    const std::string host {"ltnm.learncppthroughprojects.com"};
    const std::string port {"80"};
    const std::string message {"hello learncppthroughprojects :)"};

    net::io_context ioc {};

    tcp::resolver resolver {ioc};
    websocket::stream<boost::beast::tcp_stream> ws {ioc};
    boost::asio::const_buffer wBuffer {message.c_str(), message.size()};
    boost::beast::flat_buffer rBuffer {};

    resolver.async_resolve(host, port, [&host, &port, &message, &ws, &wBuffer, &rBuffer](auto ec, auto resolverIt){
        OnResolve(ec,resolverIt, host, port, message, ws, wBuffer, rBuffer);
    });

    ioc.run();

    return 0;
}