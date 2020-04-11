#include "arp_parser.h"

int count(FILE *fin) {
    fin = fopen("arp_table.txt", "r");
    int count = 0;
    for (char c = getc(fin); c != EOF; c = getc(fin)) {
        // iterate through the lines as long as the file is not finished
        if (c == '\n') {
            // if it is a new line
            count++;
        }
    }
    if (fseek(fin, 0L, SEEK_SET) != 0) {
        printf("File error");
        /* Handle repositioning error */
    }
    return count;
}

void read_arptable(struct arp_entry *arpTable) {
    parse_arptable *v;
    /*  parse_arptable is a structure that i made that stores
    everything that is needed for the parser.
        [struct {
        char *ip;
        char *mac;
        } parse_arptable]
    */
    FILE *fin;
    fin = fopen("arp_table.txt", "r");
    // if the file doesn't open or is empty, error
    if (fin == NULL) {
        printf("Error.");
        return;
    }
    int lines = count(fin);
    // alocate the array
    v = (parse_arptable *)malloc(lines * sizeof(parse_arptable));
    for (int i = 0; i < lines; i++) {
        v[i].ip = (char *)malloc(sizeof(char));
        v[i].mac = (char *)malloc(sizeof(char));
        // alocate for each variable the right size

        fscanf(fin, "%s %s\n", v[i].ip, v[i].mac);
        /* read from file each variable and store it
        in the right field
        */
    }
    for (int i = 0; i < lines; i++) {
        /* inet_addr function converts
        the string (char* in my case) to a standard
        IPv4 dotted decimal notation */
        arpTable[i].ip = inet_addr(v[i].ip);

        /* hwaddr_aton parses ascii and converts
        it to binary form */
        hwaddr_aton(v[i].mac, arpTable[i].mac);
    }
}