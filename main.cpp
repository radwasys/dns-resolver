#include "Resolver.h"

#include <iostream>
using namespace std;

int main() {
	string domain_name;
	cout << "Please Enter Domain Name: ";
	cin >> domain_name;
	Resolver resolver;
	string ip_addr = resolver.getIpofDomainName(domain_name);
	cout << "IP Address: " << ip_addr << endl;
  return 0;
}
