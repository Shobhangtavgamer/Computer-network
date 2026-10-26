#include <stdio.h>
#include <stdint.h>
int main()
{
     unsigned int a, b, c, d;
     uint32_t ip, mask, network, broadcast;
     int hostBits;
     char *className;
     printf("Enter IP address: ");
     if (scanf("%u.%u.%u.%u", &a, &b, &c, &d) != 4 ||
         a > 255 || b > 255 || c > 255 || d > 255)
     {
          printf("Invalid IP address!\n");
          return 1;
     }
     ip = (a << 24) | (b << 16) | (c << 8) | d;
     if (a >= 1 && a <= 126)
     {
          className = "A";
          mask = 0xFF000000;
          hostBits = 24;
     }
     else if (a >= 128 && a <= 191)
     {
          className = "B";
          mask = 0xFFFF0000;
          hostBits = 16;
     }
     else if (a >= 192 && a <= 223)
     {
          className = "C";
          mask = 0xFFFFFF00;
          hostBits = 8;
     }
     else if (a >= 224 && a <= 239)
     {
          printf("Class D: Multicast address.\n");
          return 0;
     }
     else if (a >= 240 && a <= 255)
     {
          printf("Class E: Experimental/Reserved address.\n");
          return 0;
     }
     else
     {
          printf("Reserved address (0.x.x.x).\n");
          return 0;
     }
     network = ip & mask;
     broadcast = network | ~mask;
     printf("\nIP Address: %u.%u.%u.%u\n", a, b, c, d);
     printf("Class: %s\n", className);
     printf("Default Subnet Mask: ");
     printf("%u.%u.%u.%u\n",
            (mask >> 24) & 255,
            (mask >> 16) & 255,
            (mask >> 8) & 255,
            mask & 255);

     printf("Network Address: %u.%u.%u.%u\n",
            (network >> 24) & 255,
            (network >> 16) & 255,
            (network >> 8) & 255,
            network & 255);

     printf("Broadcast Address: %u.%u.%u.%u\n",
            (broadcast >> 24) & 255,
            (broadcast >> 16) & 255,
            (broadcast >> 8) & 255,
            broadcast & 255);

     printf("Total Addresses: %llu\n",
            1ULL << hostBits);
     printf("Usable Host Addresses: %llu\n",
            (1ULL << hostBits) - 2);
     return 0;
}
