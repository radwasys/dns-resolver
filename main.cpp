#include "ResponseResolver.h"
#include "ServerContactor.h"

#include <bits/stdc++.h>
using namespace std;

int main() {
	// Sending DNS Query
	ServerContactor server_contactor;
	vector<uint8_t> dns_response = server_contactor.sendQuery();

  // Resolving DNS Response
  ResponseResolver resolver(dns_response);

  Header header = resolver.getHeader();

	vector<Record> an_records = resolver.getAnswerRecords();
	vector<Record> ns_records = resolver.getNSRecords();
	vector<Record> a_records = resolver.getARecords();
	vector<Record> aaaa_records = resolver.getDoubleAARecords();

	cout << "answer size: " << an_records.size() << endl;
	cout << "NS size: " << ns_records.size() << endl;
	cout << "A size: " << a_records.size() << endl;
	cout << "AAAA size: " << aaaa_records.size() << endl;

  return 0;
}
