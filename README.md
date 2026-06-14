DNS Resolver


DNS Server

Responsiblities:
- Get Domain Name from user
- Send DNS Query
- Receive DNS Response 
- Pass DNS Response to DNS Response Resolver


DNS Response Resolver

Responsiblities:
- Get DNS Response
- Resolve DNS Response
	- Check for errors from header DONE
	- Check for truncation from header DONE
	- Check xid is the same as Query xid DONE
	- Get DNS Records
	- Pass DNS Records to DNS Record Resolver


DNS Record Resolver

Responsiblities:
- Get DNS Record
- Resolve DNS Record
	- 

