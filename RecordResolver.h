#include <bits/stdc++.h>
#include <bitset>
#include <cstdint>
#include <vector>
using namespace std;

struct Record {
public:
	vector<vector<uint8_t>> name;
  uint16_t type;
  uint16_t class_name;
  uint32_t time_to_live;
  uint16_t data_len;
  vector<uint8_t> data;
};

class RecordResolver {
private:
	vector<uint8_t> records_bytes;
  vector<Record> an_records;
  vector<Record> ns_records;
  vector<Record> add_records;

public:
  uint16_t EightToSixteen(uint8_t x, uint8_t y) {
    string bit1 = bitset<8>(x).to_string();
    string bit2 = bitset<8>(y).to_string();
    return bitset<16>(bit1 + bit2).to_ulong();
  }

	int setRecordName(vector<uint8_t> response, int start_index, Record& record){
		uint8_t label_len = response[start_index];

		while((label_len >> 6) != 0x03 && label_len != 0x00){
			vector<uint8_t> label;
			for(int i=0; i<label_len; i++) label.push_back(response[start_index+i]);
			record.name.push_back(label);
			start_index += label_len + 1;
			label_len = response[start_index];
		}

		if((label_len >> 6) == 0x03) {
			// Go to Pointer Byte
			uint16_t addr = EightToSixteen(label_len, response[++start_index]);
			int index = (addr ^ 0xC000);
			label_len = response[index];
			while(label_len != 0x00){
					vector<uint8_t> label;
					index++;
					for(int i=0; i<label_len; i++) label.push_back(response[index+i]);
					record.name.push_back(label);
					index += label_len + 1;
					label_len = response[index];
			}
		} 
		start_index++;

		return start_index;
	}

  RecordResolver(vector<uint8_t> response, int start_index, int an_number, int ns_number,
                 int add_number) {
		records_bytes = vector<uint8_t>(response.begin()+start_index, response.end());

    int index = start_index;
		cout << "START OF RECORDS: " << index << endl;

		for(int i=start_index; i<63; i++) cout << "byte " <<dec << i << ": " << hex <<
			bitset<8>(response[i]).to_ulong() << endl;

		int next = resolveRecord(response, index);
		cout << "next index" << dec << next << endl;
		
  }

	int resolveRecord(vector<uint8_t> response, int start_index){
			Record record;
			int index = start_index;
			int index_after_name = setRecordName(response, start_index, record);

			cout << "index index_after_name" << index_after_name << endl;
			index = index_after_name;
			record.type = EightToSixteen(response[index], response[index+1]);
			record.class_name = EightToSixteen(response[index+2], response[index+3]);
			uint16_t ttl_first_byte = EightToSixteen(response[index+4], response[index+5]);
			uint16_t ttl_second_byte = EightToSixteen(response[index+6], response[index+7]);
			record.time_to_live = ((uint32_t)ttl_first_byte << 16) | ttl_second_byte;
			record.data_len = EightToSixteen(response[index+8], response[index+9]);
			
			index += 10;
		
			for(int i=0; i<record.data_len; i++, index++) {
				record.data.push_back(response[index]);
				cout << "data: " << hex << bitset<8>(response[index]).to_ulong() << endl;
			}

			// Checks
			cout << "start of name: ";
			for(auto label : record.name)
				for(auto chr : label) cout << char(chr) << " ";
			cout << endl;
			cout << "type: " << bitset<16>(record.type).to_ulong() << endl;
			cout << "class: " << bitset<16>(record.class_name).to_ulong() << endl;
			cout << "ttl: " << bitset<32>(record.time_to_live).to_ulong() << endl;
			cout << "data_len: " << bitset<16>(record.data_len).to_ulong() << endl;
			cout << "data: ";
			for(auto x : record.data)
				cout << hex << bitset<8>(x).to_ulong() << " ";
			cout << endl;

			return index;
	}

	vector<vector<uint8_t>> resolveNSData(vector<uint8_t> data, vector<uint8_t> response){
		int label_index = 0;
		uint8_t label_len = data[label_index];
		
		vector<vector<uint8_t>> resolvedData;

		while((label_len >> 6) != 0x03 && label_len != 0x00){
			vector<uint8_t> label;
			for(int i=0; i<label_len; i++) label.push_back(data[label_index+i]);
			resolvedData.push_back(label);
			label_index += label_len + 1;
			label_len = data[label_index];
		}

		if((label_len >> 6) == 0x03) {
			// Go to Pointer Byte
			uint16_t addr = EightToSixteen(label_len, response[++label_index]);
			int index = (addr ^ 0xC000);
			label_len = response[index];
			while(label_len != 0x00){
					vector<uint8_t> label;
					index++;
					for(int i=0; i<label_len; i++) label.push_back(response[index+i]);
					resolvedData.push_back(label);
					index += label_len + 1;
					label_len = response[index];
			}
		} 

		return resolvedData;
	}

  vector<Record> getAnRecords() { return an_records; }

  vector<Record> getNsRecords() { return ns_records; }

  vector<Record> getAddRecords() { return add_records; }
};
