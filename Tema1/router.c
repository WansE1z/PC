#include "arp_parser.h"
#include "parser.h"
#include "skel.h"

int main(int argc, char *argv[]) {
    packet pkt;
    u_int16_t oldCheckSum;
    in_addr_t interfaceIP;
    FILE *fin1;
    FILE *fin2;
    fin1 = fopen("rtable.txt", "r");
    fin2 = fopen("arp_table.txt", "r");

    int iphdrLen = sizeof(struct iphdr);
    int ethdrLen = sizeof(struct ether_header);
    int icmphdrLen = sizeof(struct icmphdr);
    int recv;
    int r_tableSize = countLines(fin1);
    int arp_tableSize = countLines(fin2);

    struct iphdr *ip_hdr = (struct iphdr *)(pkt.payload + ethdrLen);
    struct icmphdr *icmp_hdr =
        (struct icmphdr *)(pkt.payload + ethdrLen + iphdrLen);
    struct route_table_entry *r_table_ent =
        malloc(r_tableSize * sizeof(struct route_table_entry));
    struct route_table_entry *r_table;
    struct arp_entry *arp_table =
        malloc(arp_tableSize * sizeof(struct arp_entry));

    read_rtable(r_table_ent);
    read_arptable(arp_table);
    quickSort(r_table_ent, 0, r_tableSize - 1);
    // sort the table in order to binary search through it
    setvbuf(stdout, NULL, _IONBF, 0);
    // router_out.txt is empty

    void icmpType(int type) {
        icmp_hdr->type = type;
        ip_hdr->protocol = IPPROTO_ICMP;
        // protocol icmp => 1
        ip_hdr->tot_len = htons(iphdrLen + icmphdrLen);
        pkt.len = iphdrLen + icmphdrLen + ethdrLen;
        // update packet length
        send_packet(pkt.interface, &pkt);
        // send the packet
    }

    void icmpEchoReply() {
        icmp_hdr->type = 0;
        // message type => echoreply

        icmp_hdr->code = 0;
        icmp_hdr->un.echo.id = htons(getpid());
        // unique identifier

        icmp_hdr->checksum = 0;
        icmp_hdr->checksum = ip_checksum(icmp_hdr, icmphdrLen);

        // time to leave needs to be a relative big number
        // tested between 30-60
        ip_hdr->ttl = 50;
        ip_hdr->ihl = 5;
        ip_hdr->version = 4;
        // 4-bit IPv4 version
        ip_hdr->tos = 0;
        // IP type of service
        ip_hdr->protocol = IPPROTO_ICMP;
        // Procotol => ICMP
        ip_hdr->id = htons(getpid());
        // Unique identifier
        ip_hdr->tot_len = htons(iphdrLen + icmphdrLen);
        pkt.len = iphdrLen + icmphdrLen + ethdrLen;
        // total length
    }

    init();
    while (1) {
        recv = get_packet(&pkt);
        DIE(recv < 0, "get_message");
        /* Students will write code here */
        interfaceIP = inet_addr(get_interface_ip(pkt.interface));
        oldCheckSum = ip_checksum(ip_hdr, iphdrLen);
        if (ip_hdr->daddr == interfaceIP) {
            icmpEchoReply();
        }
        if (ip_hdr->ttl <= 1) {
            icmpType(11);
            // icmp timeout
            continue;
        }
        if (get_best_route(ip_hdr->daddr, r_tableSize, r_table_ent) == NULL) {
            icmpType(3);
            // host unreachable
            continue;
        }
        if (oldCheckSum != 0) {
            /* if there is a packet with the wrong checksum, throw the packet */
            continue;
        }
        // decrement ttl, update checksum
        ip_hdr->ttl--;
        ip_hdr->check = 0;
        ip_hdr->check = ip_checksum(ip_hdr, iphdrLen);

        /* create a new structure route_table_ent [struct route_table_entry *r_table]
        from which i will get the best route */
        r_table = get_best_route(ip_hdr->daddr, r_tableSize, r_table_ent);

        // update the interface from the packet
        pkt.interface = r_table->interface;
        // send packet to other hosts
        send_packet(pkt.interface, &pkt);
    }
    return 0;
}