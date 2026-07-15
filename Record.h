#pragma once

#include <vector>
#include <bits/stdc++.h>
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
