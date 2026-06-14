#include <bits/stdc++.h>
#include <bitset>
#include <cstdint>
#include <vector>
using namespace std;

struct Record {
	public:
		uint16_t type;
		uint16_t class_name;
		uint32_t time_to_live;
		uint16_t data_len;
		vector<uint8_t> data;
};

class RecordResolver {
	private:
		vector<Record> an_records;
		vector<Record> ns_records;
		vector<Record> add_records;

	public:
		uint16_t convertBytestoint(uint8_t x, uint8_t y) {
			string bit1 = bitset<8>(x).to_string();
			string bit2 = bitset<8>(y).to_string();
			return bitset<16>(bit1 + bit2).to_ulong();
		}

		RecordResolver(vector<uint8_t> record_bits){
			int level = 0;
			int i=0;
			while(i<record_bits.size()){
				Record rec;
				rec.class_name = convertBytestoint(record_bits[i+2], record_bits[i+3]);
				rec.type = convertBytestoint(record_bits[i+4], record_bits[i+5]);
				uint16_t first_two = convertBytestoint(record_bits[i+6], record_bits[i+7]); 
				uint16_t second_two = convertBytestoint(record_bits[i+8], record_bits[i+9]); 
				rec.time_to_live = bitset<32>(bitset<16>(first_two).to_string()+bitset<16>(second_two).to_string()).to_ulong();
				rec.data_len = convertBytestoint(record_bits[i+10], record_bits[i+11]);
				i+=11;
				for(int j=1; j<=rec.data_len; j++){
					i+=j;
					rec.data.push_back(record_bits[i]);
				}

				if(rec.type == 2) level=1;
				else if(rec.type == 1 && level==1) level=2; 

				if(level==0) an_records.push_back(rec);
				else if(level==1) ns_records.push_back(rec);
				else add_records.push_back(rec);
			}
									
		}
};
