#include <bits/stdc++.h>
#include <cstddef>
using namespace std;
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
using boost::asio::ip::udp;
#include "RecordResolver.h"

enum ServerLevel { ROOT, SECOND };

class ServerContactor {
		private:

			boost::asio::io_context io_context;
			udp::socket socket;
			int port = 53;

		public:

			ServerContactor() : socket(io_context){
				socket.open(udp::v4());
			}

			string getIPofServer(ServerLevel srvr_lvl, Record& A_Record=nullptr){
				string ip_addr = "";
				if(srvr_lvl == ROOT) ip_addr = "198.41.0.4";
				else if(srvr_lvl == SECOND) {
						for(auto label : A_Record.data){
								for(auto chr : label) ip_addr += to_string(int(bitset<8>(chr).to_ulong())); 
								ip_addr += ".";
						}
						ip_addr = ip_addr.substr(0, ip_addr.size()-1);
				}
				return ip_addr;
			}

			vector<uint8_t> formQuery(){
					
				vector<uint8_t> send_buf = {
						0x12, 0x34, // ID
						0x01, 0x00, // Flags: QR(1) - OPCODE(4) - AA(1) - TC(1) - RD(1) - ZERO(3)
												// - RCODE(4)
						0x00, 0x01, // Question Number
						0x00, 0x00, // Answer RRs
						0x00, 0x00, // Authority RRs
						0x00, 0x01, // Aditional RRs

						0x07, 0x65, 0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x03, 0x63, 0x6f, 0x6d,
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

				return send_buf;
			}

			vector<uint8_t> sendQuery(){
				string root_ip = getIPofServer();
				vector<uint8_t> send_buf = formQuery();

				udp::endpoint receiver_endpoint(boost::asio::ip::make_address(root_ip), port);

				socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

				vector<uint8_t> recv_buf(4096);
				udp::endpoint sender_endpoint;
				size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

				return recv_buf;
			}
};
