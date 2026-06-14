#include <bits/stdc++.h>
#include <bitset>
#include <cstdint>
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

class ResponseResolver {
private:
  Header header;

public:
  ResponseResolver(vector<uint8_t>::iterator begin,
                   vector<uint8_t>::iterator end) {
    vector<uint8_t> header_bits;
    for (auto it = begin; it != end; it++)
      header_bits.push_back(*it);
    header = resolveHeader(header_bits);
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

  Header resolveHeader(vector<uint8_t> header_bits) {
    Header head;
    head.xid = convertBytestoint(header_bits[0], header_bits[1]);
    uint16_t flags_int = convertBytestoint(header_bits[2], header_bits[3]);
    head.flags = resolveFlags(flags_int);
    head.question_number = convertBytestoint(header_bits[4], header_bits[5]);
    head.answer_number = convertBytestoint(header_bits[6], header_bits[7]);
    head.authority_number = convertBytestoint(header_bits[8], header_bits[9]);
    head.additional_number =
        convertBytestoint(header_bits[10], header_bits[11]);
    return head;
  }

  bool hasTrauncation() { return header.flags.isTruncated; }

  bool hasError() { return header.flags.error_code.to_string() == "0000"; }

	string getError() {
		switch (header.flags.error_code.to_ulong()){
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
};
