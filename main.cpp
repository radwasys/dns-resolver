#include <array>
#include <boost/asio.hpp>
#include <bits/stdc++.h>
#include <boost/asio/ip/address.hpp>
#include <cstdint>
using namespace std;
using boost::asio::ip::udp;

int main() {
		string ip = "198.41.0.4";
		int port = 53;

		vector<uint8_t> send_buf = {
			0x12, 0x34,		// ID 
			0x01, 0x00,		// Flags: QR(1) - OPCODE(4) - AA(1) - TC(1) - RD(1) - ZERO(3) - RCODE(4)
			0x00, 0x01, 	// Question Number
			0x00, 0x00, 	// Answer RRs
			0x00, 0x00, 	// Authority RRs
			0x00, 0x00,		// Aditional RRs
			0x07, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d, 0x00, // QNAME
			0x00, 0x01, 	// QTYPE
			0x00, 0x01		// QCLASS
		};
			
    boost::asio::io_context io_context;

    udp::endpoint receiver_endpoint(boost::asio::ip::make_address(ip), port);

    udp::socket socket(io_context);
    socket.open(udp::v4());

    socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

    std::array<uint8_t, 512> recv_buf;
    udp::endpoint sender_endpoint;
    size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
		cout << len << endl;
		int recv_id = bitset<16>(bitset<8>(recv_buf[0]).to_string()+bitset<8>(recv_buf[1]).to_string()).to_ulong();
		cout << "Request ID: " << recv_id << endl;
		cout << "Flags: " << bitset<8>(recv_buf[2]).to_string() << bitset<8>(recv_buf[3]).to_string() << endl;
		cout << "Number of Questions: " << bitset<8>(recv_buf[4]).to_string() << endl;
		cout << "Number of Answers: " << bitset<8>(recv_buf[5]).to_string() << endl;
		string question = "";
		int i=8;
		string byte = bitset<8>(recv_buf[i++]).to_string();
		cout << byte << endl;
		while(byte != "00000000"){
			int word_len = bitset<8>(bitset<8>(recv_buf[i++]).to_string()).to_ulong();
			cout << word_len << endl;
			while(word_len--){
				cout << "char: " << bitset<8>(recv_buf[i++]).to_ulong() << endl;
			}
		}
		cout << "Question: " << question << endl;
    return 0;
}
