#pragma once

#include <bits/stdc++.h>
#include <bitset>
#include <cstdint>
#include <sys/types.h>
#include <sys/ucontext.h>
using namespace std;

#include "RecordParser.h"
#include "Record.h"
#include "Header.h"

const int HEADER_BYTES = 12;

class ResponseParser {
private:
  Header header;
  vector<Record> ns_records;
	vector<Record> cn_records;
  vector<Record> a_records;
  vector<Record> aaaa_records;

public:
  ResponseParser(vector<uint8_t> response) : header(getHeaderBytes(response)){
		// Get Records
    int start_index = getStartOfRecords(response);

		// Resolve Records
    RecordParser record_parser(response, start_index, header.answer_number, header.authority_number, header.additional_number);

	  // Separate Records
    ns_records = record_parser.getNsRecords();
		cn_records = record_parser.getCNRecords();
    a_records = record_parser.getARecords();
    aaaa_records = record_parser.getAAAARecords();
  }

  vector<uint8_t> getHeaderBytes(vector<uint8_t> response) {
    vector<uint8_t> header_bytes;
    for (auto it = response.begin(); it != response.begin() + HEADER_BYTES;
         it++)
      header_bytes.push_back(*it);
    return header_bytes;
  }

  int getStartOfRecords(vector<uint8_t> response) {
		int start_index = HEADER_BYTES;
		int label_len = response[HEADER_BYTES];
		while(label_len != 0x00){
				start_index += label_len + 1;
				label_len = response[start_index];
		}
		start_index += 4; // For Class and Type
    return start_index+1;
  }


	void printRecords(){
		for(auto record : ns_records){
			cout << "===============NS RECORD===============" << endl;
			cout << "Record name: ";
			for(auto label : record.name){
				 for(auto chr : label)
					 cout << char(bitset<8>(chr).to_ulong());
				 cout << ".";
			}
			cout << endl;
			cout << "type: " << bitset<16>(record.type).to_ulong() << endl;
			cout << "class: " << bitset<16>(record.class_name).to_ulong() << endl;
			cout << "ttl: " << bitset<32>(record.time_to_live).to_ulong() << endl;
			cout << "data_len: " << bitset<16>(record.data_len).to_ulong() << endl;
			cout << "Data: ";
			for(auto label : record.data){
				 for(auto chr : label)
					 cout << char(bitset<8>(chr).to_ulong());
				 cout << ".";
			}
			cout << endl;
		}

		for(auto record : cn_records){
			cout << "===============CNAME RECORD===============" << endl;
			cout << "Record name: ";
			for(auto label : record.name){
				 for(auto chr : label)
					 cout << char(bitset<8>(chr).to_ulong());
				 cout << ".";
			}
			cout << endl;
			cout << "type: " << bitset<16>(record.type).to_ulong() << endl;
			cout << "class: " << bitset<16>(record.class_name).to_ulong() << endl;
			cout << "ttl: " << bitset<32>(record.time_to_live).to_ulong() << endl;
			cout << "data_len: " << bitset<16>(record.data_len).to_ulong() << endl;
			cout << "Data: ";
			for(auto label : record.data){
				 for(auto chr : label)
					 cout << char(bitset<8>(chr).to_ulong());
				 cout << ".";
			}
			cout << endl;
		}



		for(auto record : a_records){
			cout << "===============A RECORD===============" << endl;
			cout << "Record name: ";
			for(auto label : record.name){
				for(auto chr : label)
					cout << char(bitset<8>(chr).to_ulong());
				cout << ".";
			}
			cout << endl;
			cout << "type: " << bitset<16>(record.type).to_ulong() << endl;
			cout << "class: " << bitset<16>(record.class_name).to_ulong() << endl;
			cout << "ttl: " << bitset<32>(record.time_to_live).to_ulong() << endl;
			cout << "data_len: " << bitset<16>(record.data_len).to_ulong() << endl;
			cout << "Data: ";
			for(auto label : record.data){
				 for(auto chr : label)
					 cout << dec << bitset<8>(chr).to_ulong();
				 cout << ".";
			}
			cout << endl;
		}

	}


  bool hasTrauncation() { return header.flags.isTruncated; }

  bool hasError() { return header.flags.error_code.to_string() == "0000"; }

  string getError() {
    switch (header.flags.error_code.to_ulong()) {
    case 0:
      return "NOERROR";
    case 1:
      return "FORMERROR";
    case 2:
      return "SERVFAIL";
    case 3:
      return "NXDOMAIN";
    case 4:
      return "NOTIMP";
    case 5:
      return "REFUSED";
    }
  }

  bool hasSameID(uint16_t id) { return header.xid == id; }

  Header getHeader() { return header; }

  vector<Record> getNSRecords() { return ns_records; }

  vector<Record> getCNRecords() { return cn_records; }

  vector<Record> getDoubleAARecords() { return aaaa_records; }

  vector<Record> getARecords() { return a_records; }
};
