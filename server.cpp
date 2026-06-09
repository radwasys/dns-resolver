#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

int main() {
    net::io_context ioc;
    tcp::acceptor acceptor(ioc, tcp::endpoint(tcp::v4(), 8080));

    while (true) {
        tcp::socket socket(ioc);
        acceptor.accept(socket);

        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);

        http::response<http::string_body> res(http::status::ok, req.version());
        res.set(http::field::content_type, "text/plain");
        res.body() = "Hello, world!";
        res.prepare_payload();

        http::write(socket, res);
    }

    return 0;
}
