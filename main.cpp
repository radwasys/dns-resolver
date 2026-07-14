#include "ResponseResolver.h"
#include "ServerContactor.h"

#include <bits/stdc++.h>
using namespace std;

int main() {
	string domain_name = "www.wikipedia.org";

	// Sending DNS Query to Root Server
	string root_ip_addr ="198.41.0.4"; 
	ServerContactor server_contactor(domain_name);
	vector<uint8_t> dns_response = server_contactor.sendQuery(root_ip_addr);

  // Resolving DNS Response
  ResponseResolver resolver(dns_response);

  Header header = resolver.getHeader();

	vector<Record> cn_records = resolver.getCNRecords();
	vector<Record> ns_records = resolver.getNSRecords();
	vector<Record> a_records = resolver.getARecords();
	vector<Record> aaaa_records = resolver.getDoubleAARecords();

	// Get IPV4 Address
	Record second_lvl = *(--a_records.end()); 
	string ns_server = "";
	for(auto label : second_lvl.name){
			for(auto x : label)
				ns_server += char(bitset<8>(x).to_ulong());
			ns_server += ".";
	}
	ns_server = ns_server.substr(0, ns_server.length()-1);

	string ip_addr = "";
	for(auto label : second_lvl.data){
			for(auto x : label)
				ip_addr += to_string(bitset<8>(x).to_ulong());
			ip_addr += ".";
	}
	ip_addr = ip_addr.substr(0, ip_addr.length()-1);


	// Send DNS Request to NS Server
	dns_response = server_contactor.sendQuery(ip_addr);
  ResponseResolver second_resolver(dns_response);

  header = second_resolver.getHeader();

	cn_records = second_resolver.getCNRecords();
	ns_records = second_resolver.getNSRecords();
	a_records = second_resolver.getARecords();
	aaaa_records = second_resolver.getDoubleAARecords();

	// Get IPV4 Address
	Record third_lvl = *(--a_records.end()); 
	ns_server = "";
	for(auto label : third_lvl.name){
			for(auto x : label)
				ns_server += char(bitset<8>(x).to_ulong());
			ns_server += ".";
	}
	ns_server = ns_server.substr(0, ns_server.length()-1);

	ip_addr = "";
	for(auto label : third_lvl.data){
			for(auto x : label)
				ip_addr += to_string(bitset<8>(x).to_ulong());
			ip_addr += ".";
	}
	ip_addr = ip_addr.substr(0, ip_addr.length()-1);
	
	cout << "NS Server: " << ns_server << endl;
	cout << "IPV4 Address: " << ip_addr << endl;

	// Send DNS Request to NS Server
	dns_response = server_contactor.sendQuery(ip_addr);
  ResponseResolver third_resolver(dns_response);

  header = third_resolver.getHeader();

	cn_records = third_resolver.getCNRecords();
	ns_records = third_resolver.getNSRecords();
	a_records = third_resolver.getARecords();
	aaaa_records = third_resolver.getDoubleAARecords();

	third_resolver.printRecords();

	// Get IPV4 Address

	cout << "CNAME size: " << cn_records.size() << endl;
	cout << "NS size: " << ns_records.size() << endl;
	cout << "A size: " << a_records.size() << endl;
	cout << "AAAA size: " << aaaa_records.size() << endl;

  return 0;
}
