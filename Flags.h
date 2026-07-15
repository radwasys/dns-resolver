#pragma once

#include <bitset>
#include <bits/stdc++.h>
using namespace std;

class Flags {
	public:
		bool isResponse;
		bitset<4> opcode;
		bool isAuthoritative;
		bool isTruncated;
		bool recursionDesired;
		bool recursionAvailable;
		bitset<4> error_code;

  	Flags(uint16_t flags) {
			string flags_str = bitset<16>(flags).to_string();
			isResponse = (flags_str[0] == '1');
			opcode = bitset<4>(flags_str.substr(1, 4));
			isAuthoritative = (flags_str[5] == '1');
			isTruncated = (flags_str[6] == '1');
			recursionDesired = (flags_str[7] == '1');
			recursionAvailable = (flags_str[8] == '1');
			error_code = bitset<4>(flags_str.substr(12, 4));
		}

		Flags(){
				isResponse = false;
				opcode = bitset<4>(0);
				isAuthoritative = false;
				isTruncated = false;
				recursionAvailable = false;
				recursionDesired = false;
				error_code = bitset<4>(0);
		}
};
