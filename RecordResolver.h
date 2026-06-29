#include <bits/stdc++.h>
#include <bitset>
#include <cstdint>
#include <filesystem>
#include <vector>
using namespace std;

struct Record {
public:
	vector<vector<uint8_t>> name;
  uint16_t type;
  uint16_t class_name;
  uint32_t time_to_live;
  uint16_t data_len;
  vector<vector<uint8_t>> data;
};

class RecordResolver {
private:
	vector<uint8_t> records_bytes;
  vector<Record> an_records;
  vector<Record> ns_records;
	vector<Record> a_records;
	vector<Record> aaaa_records;

public:
  uint16_t EightToSixteen(uint8_t x, uint8_t y) {
    string bit1 = bitset<8>(x).to_string();
    string bit2 = bitset<8>(y).to_string();
    return bitset<16>(bit1 + bit2).to_ulong();
  }

	int followPointer(vector<uint8_t> response, int start_index, vector<vector<uint8_t>>& result){
		uint8_t label_len = response[start_index];

		while((label_len >> 6) != 0x03 && label_len != 0x00){
			vector<uint8_t> label;
			for(int i=1; i<=label_len; i++) label.push_back(response[start_index+i]);
			result.push_back(label);
			start_index += label_len + 1;
			label_len = response[start_index];
		}
		
		if((label_len >> 6) == 0x03){
			uint16_t addr = EightToSixteen(label_len, response[++start_index]);
			int ptr_index = (addr ^ 0xC000);
			int dummy = followPointer(response, ptr_index, result);
		}
		return start_index + 1;
		
	}


  RecordResolver(vector<uint8_t> response, int start_index, int an_number, int ns_number,
                 int add_number) {

		int total_records = an_number + ns_number + add_number;
		records_bytes = vector<uint8_t>(response.begin()+start_index, response.end());
    int index = start_index;

		cout << "START OF RECORDS: " << index << endl;

		for(int i=0; i < ns_number+add_number; i++){
			Record record;
			index = resolveRecord(response, index, record);
		}	
		printRecords();
  }

	int resolveRecord(vector<uint8_t> response, int start_index, Record& record){
			int index = start_index;

			vector<vector<uint8_t>> name_vec;
			index = followPointer(response, start_index, name_vec);
			
			record.name = name_vec;
			record.type = EightToSixteen(response[index], response[index+1]);
			record.class_name = EightToSixteen(response[index+2], response[index+3]);
			uint16_t ttl_first_byte = EightToSixteen(response[index+4], response[index+5]);
			uint16_t ttl_second_byte = EightToSixteen(response[index+6], response[index+7]);
			record.time_to_live = ((uint32_t)ttl_first_byte << 16) | ttl_second_byte;
			record.data_len = EightToSixteen(response[index+8], response[index+9]);
			
			index += 10;
		
			vector<uint8_t> unresolved_data;
			for(int i=0; i<record.data_len; i++, index++) {
				unresolved_data.push_back(response[index]);
			}

			if(record.type == 2){ // NS Record
				record.data = resolveNSData(unresolved_data, response);
				ns_records.push_back(record);
			} else if(record.type == 1){
				record.data = resolveAData(unresolved_data, response);
				a_records.push_back(record);
			} else if(record.type == 28){
				record.data = resolveAData(unresolved_data, response);
				aaaa_records.push_back(record);
			}
			else cout << "UNIDENTIFIED: " << dec << record.type << endl;

			return index;
	}

	vector<vector<uint8_t>> resolveNSData(vector<uint8_t> data, vector<uint8_t> response){
		int label_index = 0;
		uint8_t label_len = data[label_index];
		
		vector<vector<uint8_t>> resolvedData;

		while((label_len >> 6) != 0x03 && label_len != 0x00){
			vector<uint8_t> label;
			for(int i=1; i<=label_len; i++) label.push_back(data[label_index+i]);
			resolvedData.push_back(label);
			label_index += label_len + 1;
			label_len = data[label_index];
		}

		if((label_len >> 6) == 0x03) {
			// Go to Pointer Byte
			uint16_t addr = EightToSixteen(label_len, data[++label_index]);
			int index = (addr ^ 0xC000);
			label_len = response[index];
			while(label_len != 0x00){
					vector<uint8_t> label;
					for(int i=1; i<=label_len; i++) label.push_back(response[index+i]);
					resolvedData.push_back(label);
					index += label_len + 1;
					label_len = response[index];
			}
		} 

		return resolvedData;
	}

	vector<vector<uint8_t>> resolveAData(vector<uint8_t> data, vector<uint8_t> response){
			vector<vector<uint8_t>> resolvedData;
			for(auto section : data){
				resolvedData.push_back({section});
			}
			return resolvedData;
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

		for(auto record : aaaa_records){
			cout << "===============AAAA RECORD===============" << endl;
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

  vector<Record> getAnRecords() { return an_records; }

  vector<Record> getNsRecords() { return ns_records; }

  vector<Record> getARecords() { return a_records; }

  vector<Record> getAAAARecords() { return aaaa_records; }
};
