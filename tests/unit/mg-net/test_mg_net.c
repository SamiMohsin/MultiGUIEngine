/**
 * @file test_mg_net.c
 * @brief Comprehensive headless unit tests for mg-net.
 */
#include "mg/net/net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int test_net_host_connect_and_send(void) {
    printf("Testing network host connect, packet transmission, and polling...\n");

    mg_net_host_t* host = mg_net_host_create(7777, 16);
    if (!host) return 1;

    mg_peer_handle_t peer = mg_net_connect(host, "127.0.0.1", 7777);
    if (peer == MG_INVALID_PEER) {
        fprintf(stderr, "FAIL: connect returned invalid peer\n");
        mg_net_host_destroy(host);
        return 1;
    }

    /* First poll should yield connect event */
    mg_net_event_t ev;
    if (!mg_net_poll(host, &ev) || ev.type != MG_NET_EVENT_CONNECT) {
        fprintf(stderr, "FAIL: expected connect event\n");
        mg_net_host_destroy(host);
        return 1;
    }

    const char* msg = "Hello MultiGUIEngine Network!";
    bool sent = mg_net_send(host, peer, MG_NET_RELIABLE_ORDERED, msg, strlen(msg) + 1);
    if (!sent) {
        fprintf(stderr, "FAIL: mg_net_send returned false\n");
        mg_net_host_destroy(host);
        return 1;
    }

    /* Next poll should yield receive event */
    if (!mg_net_poll(host, &ev) || ev.type != MG_NET_EVENT_RECEIVE) {
        fprintf(stderr, "FAIL: expected receive event\n");
        mg_net_host_destroy(host);
        return 1;
    }

    if (strcmp((const char*)ev.data, msg) != 0) {
        fprintf(stderr, "FAIL: packet payload mismatch (%s != %s)\n", (const char*)ev.data, msg);
        mg_net_host_destroy(host);
        return 1;
    }

    mg_net_disconnect(host, peer);
    if (!mg_net_poll(host, &ev) || ev.type != MG_NET_EVENT_DISCONNECT) {
        fprintf(stderr, "FAIL: expected disconnect event\n");
        mg_net_host_destroy(host);
        return 1;
    }

    mg_net_host_destroy(host);
    printf("PASS: network host connect, send, and poll\n");
    return 0;
}

int main(void) {
    if (test_net_host_connect_and_send() != 0) return 1;

    printf("All mg-net unit tests passed successfully!\n");
    return 0;
}
