CC = g++
ALL = dns-export
CFLAGS = -Werror -Wextra -Wall -pedantic -std=c++11
LDFLAGS=-lpcap
OFILES = dns-export.o sniffer.o headers.o dns_parser.o stats_report.o message_reassembler.o base64.o
LINK.o = $(LINK.cpp)


all: dns-export

dns-export: $(OFILES)
	$(CC) $(CFLAGS) $? -o $@ $(LDFLAGS)

dns-export.o: dns-export.cpp dns-export.hpp sniffer.hpp
	$(CC) $(CFLAGS) -c $< -o $@

sniffer.o: sniffer.cpp sniffer.hpp dns-export.hpp headers.hpp dns_parser.hpp stats_report.hpp
	$(CC) $(CFLAGS) -c $< -o $@

headers.o: headers.cpp headers.hpp message_reassembler.hpp
	$(CC) $(CFLAGS) -c $< -o $@

dns_parser.o: dns_parser.cpp dns_parser.hpp headers.hpp base64.hpp
	$(CC) $(CFLAGS) -c $< -o $@

stats_report.o: stats_report.cpp stats_report.hpp
	$(CC) $(CFLAGS) -c $< -o $@

message_reassembler.o: message_reassembler.cpp message_reassembler.hpp
	$(CC) $(CFLAGS) -c $< -o $@

base64.o: base64.cpp base64.hpp
	$(CC) $(CFLAGS) -c $< -o $@
	
.phony: clean

clean:
	rm $(ALL) $(OFILES)
