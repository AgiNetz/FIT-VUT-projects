# Network applications and network administration project - DNS information export using Syslog

The goal of this project was to implement a tool for monitoring incoming DNS traffic, analysing it, storing statistics about it and sending them to a Syslog server. The tool generates statistics in the following format:\
\
domain-name rr-type rr-answer count\
\
where *domain-name* is the domain name in question, *rr-type* is a type of a resource record being transmitted, *rr-answer* is an *rr-type* dependent payload of the message and *count* is the number of times this message has been seen. For further information, refer to the manual.pdf file.
