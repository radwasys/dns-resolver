#include "RecordResolver.h"
#include <bits/stdc++.h>
#include <bitset>
#include <cstdint>
#include <sys/types.h>
#include <sys/ucontext.h>
using namespace std;

struct Flags {
  bool isResponse;
  bitset<4> opcode;
  bool isAuthoritative;
  bool isTruncated;
  bool recursionDesired;
  bool recursionAvailable;
  bitset<4> error_code;
};

struct Header {
  uint16_t xid;
  Flags flags;
  uint16_t question_number;
  uint16_t answer_number;
  uint16_t authority_number;
  uint16_t additional_number;
};

const int HEADER_BYTES = 12;

class ResponseResolver {
private:
  Header header;
  vector<Record> an_records;
  vector<Record> ns_records;
  vector<Record> add_records;

public:
  ResponseResolver(vector<uint8_t> response) {
		// Get Header
    vector<uint8_t> header_bytes = getHeaderBytes(response);
    header = resolveHeader(header_bytes);
		// Get Records
    int start_index = getStartOfRecords(response);
		// Resolve Records
		cout << "check before records" << endl;
    RecordResolver record_resolver(response, start_index, header.answer_number, header.authority_number, header.additional_number);
	  cout << "check after records" << endl;
	  // Separate Records
    an_records = record_resolver.getAnRecords();
    ns_records = record_resolver.getNsRecords();
    add_records = record_resolver.getAddRecords();
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

  uint16_t convertBytestoint(uint8_t x, uint8_t y) {
    string bit1 = bitset<8>(x).to_string();
    string bit2 = bitset<8>(y).to_string();
    return bitset<16>(bit1 + bit2).to_ulong();
  }

  Flags resolveFlags(uint16_t flags) {
    string flags_str = bitset<16>(flags).to_string();
    Flags flag_final;
    flag_final.isResponse = (flags_str[0] == '1');
    flag_final.opcode = bitset<4>(flags_str.substr(1, 4));
    flag_final.isAuthoritative = (flags_str[5] == '1');
    flag_final.isTruncated = (flags_str[6] == '1');
    flag_final.recursionDesired = (flags_str[7] == '1');
    flag_final.recursionAvailable = (flags_str[8] == '1');
    flag_final.error_code = bitset<4>(flags_str.substr(12, 4));
    return flag_final;
  }

  Header resolveHeader(vector<uint8_t> header_bytes) {
    Header head;
    head.xid = convertBytestoint(header_bytes[0], header_bytes[1]);
    uint16_t flags_int = convertBytestoint(header_bytes[2], header_bytes[3]);
    head.flags = resolveFlags(flags_int);
    head.question_number = convertBytestoint(header_bytes[4], header_bytes[5]);
    head.answer_number = convertBytestoint(header_bytes[6], header_bytes[7]);
    head.authority_number = convertBytestoint(header_bytes[8], header_bytes[9]);
    head.additional_number =
        convertBytestoint(header_bytes[10], header_bytes[11]);
    return head;
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

  vector<Record> getAnswerRecords() { return an_records; }

  vector<Record> getNSRecords() { return ns_records; }

  vector<Record> getAdditionalRecords() { return add_records; }
};
