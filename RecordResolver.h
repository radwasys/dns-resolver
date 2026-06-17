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
  uint8_t data_len;
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

  int getNameLength(vector<uint8_t> record_bits, int starting_index) {
    string byte =
        bitset<8>(record_bits[starting_index]).to_string().substr(0, 2);
    if (byte == "11")
      return 2;

    int label_len;
    int index = starting_index;
    while (byte != "11" && record_bits[index] != 0x00) {
      label_len = record_bits[index];
      index += label_len + 1;
      byte = bitset<8>(record_bits[index]).to_string().substr(0, 2);
    }
    if (byte == "11")
      return index - starting_index + 2;
    return index - starting_index + 1;
  }

  RecordResolver(vector<uint8_t> record_bits, int an_number, int ns_number,
                 int add_number) {
    cout << "an=" << an_number << endl;
    cout << "ns=" << ns_number << endl;
    cout << "add=" << add_number << endl;
    int index = 0;
    int total_records = an_number + ns_number + add_number;
    cout << "total=" << total_records << endl;
    for (int i = 0; i < total_records; i++) {
      cout << "Record: " << i << endl;
      cout << "index = " << index << endl;
      cout << "size = " << record_bits.size() << endl;
      cout << hex << (int)record_bits[index] << " "
           << (int)record_bits[index + 1] << dec << endl;
      int name_len = getNameLength(record_bits, index);
      cout << "name_len = " << name_len << endl;
      index += name_len;
      Record rec;
      rec.type = convertBytestoint(record_bits[index], record_bits[index + 1]);
      rec.class_name =
          convertBytestoint(record_bits[index + 2], record_bits[index + 3]);
      uint16_t first_two =
          convertBytestoint(record_bits[index + 4], record_bits[index + 5]);
      uint16_t second_two =
          convertBytestoint(record_bits[index + 6], record_bits[index + 7]);
      rec.time_to_live = bitset<32>(bitset<16>(first_two).to_string() +
                                    bitset<16>(second_two).to_string())
                             .to_ulong();
      rec.data_len =
          convertBytestoint(record_bits[index + 8], record_bits[index + 9]);
      cout << "after name index = " << index << endl;
      cout << "need bytes up to " << (index + 9) << endl;
      cout << "size = " << record_bits.size() << endl;
      cout << (int)record_bits[index + 8] << " " << (int)record_bits[index + 9]
           << endl;
      index += 10;
      for (int j = index; j < index + rec.data_len; j++)
        rec.data.push_back(record_bits[j]);
      index += rec.data_len;

      if (i < an_number)
        an_records.push_back(rec);
      else if (i >= an_number && i < ns_number)
        ns_records.push_back(rec);
      else
        add_records.push_back(rec);
      cout << "end index: " << index << endl;
    }
  }

  vector<Record> getAnRecords() { return an_records; }

  vector<Record> getNsRecords() { return ns_records; }

  vector<Record> getAddRecords() { return add_records; }
};
