#pragma once

#include <bits/stdc++.h>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <sstream>
using namespace std;

#include <boost/asio.hpp>
#include <boost/asio/ip/address.hpp>
using boost::asio::ip::udp;

#include "ResponseParser.h"
#include "Record.h"

enum ServerLevel { ROOT, SECONDARY };

class Resolver {
		private:

			boost::asio::io_context io_context;
			udp::socket socket;
			const int port = 53;
			const int question_index = 12;
			vector<uint8_t> domain_name_bytes;
			bool isDomainNameSet = false;

			Header current_header;
			vector<Record> a_records;
			vector<Record> cn_records;
			vector<Record> ns_records;

			vector<uint8_t> query_template = {
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

			Resolver() : socket(io_context), current_header(){
				socket.open(udp::v4());
			}

			string getIpofDomainName(string domain_name){
				string root_address = getIpAddress(ServerLevel::ROOT);
				string final_address = resolveDomainName(ServerLevel::ROOT, domain_name, root_address);
				return final_address;
			}

			string resolveDomainName(ServerLevel srvr_lvl, string domain_name, string ip_address){
				vector<uint8_t> send_buf = setDomainName(domain_name);
				sendQuery(ip_address, send_buf);
				
				cout << "=============================" << endl;
				cout << "DOMAIN NAME: " << domain_name << endl;
				cout << "IP ADDRESS: " << ip_address << endl;
				cout << "ANSWER: " << current_header.answer_number << endl;
				cout << "CNAME: " << cn_records.size() << endl;
				cout << "NS: " << ns_records.size() << endl;
				cout << "A: " << a_records.size() << endl;
				cout << "============================" << endl;

				if(current_header.answer_number != 0 && cn_records.size() > 0)
					return resolveDomainName(ServerLevel::SECONDARY, getDataFromRecord(cn_records[0]), ip_address);
				else if(current_header.answer_number != 0 && a_records.size() > 0)
					return getDataFromRecord(a_records[0]);
				else if(current_header.answer_number == 0 && a_records.size() > 0)
					return resolveDomainName(ServerLevel::SECONDARY, domain_name, getDataFromRecord(a_records[0]));
				else if(current_header.answer_number == 0 && a_records.size() == 0){
					string ns_address = resolveDomainName(ServerLevel::ROOT, getDataFromRecord(ns_records[0]), getIpAddress(ServerLevel::ROOT));
					return resolveDomainName(ServerLevel::SECONDARY, domain_name, ns_address);
				}
				return "NOT FOUND";
			}

			vector<uint8_t> setDomainName(string domain_name){
				stringstream ss(domain_name);
				string label;
				vector<uint8_t> dn_bytes;

				// Convert Domain Name to Bytes
				while(getline(ss, label, '.')){
					dn_bytes.push_back(label.size());
					for(auto chr : label)
						dn_bytes.push_back(int(chr));
				}
				domain_name_bytes = dn_bytes;

				// Add Domain Name to Query
				vector<uint8_t> send_buf = query_template;
				if(isDomainNameSet)
					send_buf.erase(send_buf.begin() + question_index, send_buf.begin() + question_index + domain_name_bytes.size());
				send_buf.insert(send_buf.begin() + question_index, dn_bytes.begin(), dn_bytes.end());

				return send_buf; 
			}

			string getIpAddress(ServerLevel srvr_lvl){
				string ip_address;
				if(srvr_lvl == ServerLevel::ROOT){
					ifstream root_file("root-servers.txt");
					getline(root_file, ip_address);
					root_file.close();
				} else {
					ip_address = getDataFromRecord(a_records[0]);	
				}
				return ip_address;
			}

			string getDataFromRecord(Record record){
				string data_str = "";
				for(auto label : record.data){
						for(auto x : label){
							if(record.type == 1)
								data_str += to_string(bitset<8>(x).to_ulong());
							else if(record.type == 5 || record.type == 2)
								data_str += char(bitset<8>(x).to_ulong());
						}
						data_str += ".";
				}
				data_str = data_str.substr(0, data_str.length()-1);
				return data_str;
			}

			vector<uint8_t> sendQuery(string ip_address, vector<uint8_t> send_buf){
				udp::endpoint receiver_endpoint(boost::asio::ip::make_address(ip_address), port);
				socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

				vector<uint8_t> recv_buf(4096);
				udp::endpoint sender_endpoint;
				size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

				ResponseParser parser(recv_buf);
				current_header = parser.getHeader();
				a_records = parser.getARecords();
				cn_records = parser.getCNRecords();
				ns_records = parser.getNSRecords();

				return recv_buf;
			}

};
