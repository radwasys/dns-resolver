#include <vector>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
using namespace std;

using boost::asio::ip::tcp;

int main() {
		string ip_addr = "104.154.89.105";
		string host = "http.badssl.com";
		int port = 80;

    boost::asio::io_context io_context;

		tcp::endpoint endpoints(boost::asio::ip::make_address(ip_addr), port);

    tcp::socket socket(io_context);
		socket.connect(endpoints);

		string request = "GET / HTTP/1.1\r\nHost: ";
		request += host + "\r\n";
		request += "\r\n";
		cout << request;
		boost::asio::write(socket, boost::asio::buffer(request));

    while (true) {
        vector<char> recv_buf(4096);
        boost::system::error_code error;

        size_t len = socket.read_some(boost::asio::buffer(recv_buf), error);

        if (error == boost::asio::error::eof)
            break;
        else if (error)
            throw boost::system::system_error(error);

        std::cout.write(recv_buf.data(), len);
    }

    return 0;
}
