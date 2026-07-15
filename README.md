DNS Resolver (Right now a terminal app, later maybe a server)

Resolving Process:

- Get domain name from user through input
- Recrusively until number of answer records isn't zero
    - Get ip address from "root-server.txt"
    - Send Query to ip address
    - Parse Response using ResponseResolver and RecordResolver
        - Answer Records = 0:
            - Additional Records > 0: get ip address 
            - Additional Records = 0: get ns server from authority records
        - Answer Records > 0:
            - A Record: final ip address
            - CNAME Record: get new domain name
