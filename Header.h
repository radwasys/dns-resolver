#pragma once

#include <bits/stdc++.h>
using namespace std;

#include "Flags.h"

class Header {
	public:
		uint16_t xid;
		Flags flags;
		uint16_t question_number;
		uint16_t answer_number;
		uint16_t authority_number;
		uint16_t additional_number;
		
		uint16_t convertBytestoint(uint8_t x, uint8_t y) {
			string bit1 = bitset<8>(x).to_string();
			string bit2 = bitset<8>(y).to_string();
			return bitset<16>(bit1 + bit2).to_ulong();
		}

		Header(vector<uint8_t> header_bytes) : 
			flags(convertBytestoint(header_bytes[2], header_bytes[3])) {

			xid = convertBytestoint(header_bytes[0], header_bytes[1]);
			question_number = convertBytestoint(header_bytes[4], header_bytes[5]);
			answer_number = convertBytestoint(header_bytes[6], header_bytes[7]);
			authority_number = convertBytestoint(header_bytes[8], header_bytes[9]);
			additional_number = convertBytestoint(header_bytes[10], header_bytes[11]);
		}

		Header() : flags(){
			xid = 0;
			question_number = 0;
			answer_number = 0;
			authority_number = 0;
			additional_number = 0;
		}
};
