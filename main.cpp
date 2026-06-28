#include "ResponseResolver.h"
#include <bits/stdc++.h>
#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
#include <cstdint>
#include <vector>
using namespace std;
using boost::asio::ip::udp;

int main() {
  string ip = "198.41.0.4";
  int port = 53;

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

  boost::asio::io_context io_context;

  udp::endpoint receiver_endpoint(boost::asio::ip::make_address(ip), port);

  udp::socket socket(io_context);
  socket.open(udp::v4());

  socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

  vector<uint8_t> recv_buf(512);
  udp::endpoint sender_endpoint;
  size_t len =
      socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

  // Resolving DNS Response
  ResponseResolver resolver(recv_buf);
  Header header = resolver.getHeader();
  cout << "answer size from header: " << header.answer_number << endl;
  cout << "ns size from header: " << header.authority_number << endl;
  cout << "additonal size from header: " << header.additional_number << endl;

	vector<Record> an_records = resolver.getAnswerRecords();
	vector<Record> ns_records = resolver.getNSRecords();
	vector<Record> add_records = resolver.getAdditionalRecords();
	cout << "answer size: " << an_records.size() << endl;
	cout << "NS size: " << ns_records.size() << endl;
	cout << "additional size: " << add_records.size() << endl;
  // cout << std::hex << header.xid << endl;
  // cout << std::dec << header.question_number << endl;
  // cout << header.flags.isResponse << endl;
  // cout << header.flags.opcode << endl;
  // cout << header.flags.isTruncated << endl;
  // cout << header.flags.recursionAvailable << endl;
  // cout << header.flags.recursionDesired << endl;
  // cout << header.flags.error_code << endl;
  return 0;
}
