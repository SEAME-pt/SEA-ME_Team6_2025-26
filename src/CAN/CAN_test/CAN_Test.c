#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <arpa/inet.h>

int main() {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;

    // Open CAN RAW socket
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("socket");
        return 1;
    }

    // Specify CAN interface
    strcpy(ifr.ifr_name, "can0");
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl");
        return 1;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    printf("CAN socket bound to can0\n");

    // Simple send/receive loop
    while (1) {
        // Send 1-byte frame with CAN ID 0x100
        frame.can_id = 0x100;
        frame.can_dlc = 1;
        frame.data[0] = 0x01;
        if (write(s, &frame, sizeof(frame)) != sizeof(frame)) {
            perror("write");
        } else {
            printf("Sent CAN ID 0x100 data 0x01\n");
        }

        // Read any frame
        int nbytes = read(s, &frame, sizeof(frame));
        if (nbytes > 0) {
            printf("Received CAN ID 0x%X len=%d data=", frame.can_id, frame.can_dlc);
            for (int i = 0; i < frame.can_dlc; i++)
                printf("%02X ", frame.data[i]);
            printf("\n");
        }

        sleep(1);
    }

    close(s);
    return 0;
}