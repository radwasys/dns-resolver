#include <bits/stdc++.h>
#include <cstddef>
#include <cstdint>
#include <sstream>
using namespace std;
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
using boost::asio::ip::udp;

enum ServerLevel { ROOT, SECOND };

class ServerContactor {
		private:

			boost::asio::io_context io_context;
			udp::socket socket;
			int port = 53;
			int question_index = 12;
			vector<uint8_t> send_buf = {
					0x12, 0x34, // ID
					0x01, 0x00, // Flags: QR(1) - OPCODE(4) - AA(1) - TC(1) - RD(1) - ZERO(3)
											// - RCODE(4)
					0x00, 0x01, // Question Number
					0x00, 0x00, // Answer RRs
					0x00, 0x00, // Authority RRs
					0x00, 0x01, // Aditional RRs
					
					// QUESTION PLACE
					0x00,       // QNAME
					0x00, 0x01, // QTYPE
					0x00, 0x01, // QCLASS

					0x00,       // NAME = root
					0x00, 0x29, // TYPE = OPT (41)

					0x04, 0xD0, // UDP payload size = 1232

					0x00, // Extended RCODE
					0x00, // EDNS Version

					0x00, 0x00, // EDNS Flags

					0x00, 0x00 // RDLEN = 0
			};

		public:

			ServerContactor(string domain_name) : socket(io_context){
				socket.open(udp::v4());
				DNtoBytes(domain_name);
			}

			void DNtoBytes(string domain_name){
				stringstream ss(domain_name);
				string label;
				vector<uint8_t> dn_bytes;

				while(getline(ss, label, '.')){
					dn_bytes.push_back(label.size());
					for(auto chr : label)
						dn_bytes.push_back(int(chr));
				}

				send_buf.insert(send_buf.begin() + question_index, dn_bytes.begin(), dn_bytes.end());
			}

			vector<uint8_t> sendQuery(string ip_addr){
				udp::endpoint receiver_endpoint(boost::asio::ip::make_address(ip_addr), port);

				socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

				vector<uint8_t> recv_buf(4096);
				udp::endpoint sender_endpoint;
				size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

				return recv_buf;
			}
};
