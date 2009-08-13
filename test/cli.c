#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enet/enet.h"
int main(int argc, char**argv) {
    if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }

   ENetHost * client;

    client = enet_host_create (NULL /* create a client host */,
                1 /* only allow 1 outgoing connection */,
                57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
                14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);

    if (client == NULL)
    {
        fprintf (stderr, 
                 "An error occurred while trying to create an ENet client host.\n");
        exit (EXIT_FAILURE);
    }
    ENetAddress address;
    ENetEvent event;
    ENetPeer *peer;

    /* Connect to some.server.net:1234. */
    enet_address_set_host (& address, "localhost");
    address.port = 1234;

    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect (client, & address, 32767);    
    
    if (peer == NULL)
    {
       fprintf (stderr, 
                "No available peers for initiating an ENet connection.\n");
       exit (EXIT_FAILURE);
    }
    
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service (client, & event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        puts ("Connection to some.server.net:1234 succeeded.");
        ENetPacket * packet = enet_packet_create ("packet", 
                                                  strlen ("packet") + 1, 
                                                  ENET_PACKET_FLAG_RELIABLE);
        
        /* Extend the packet so and append the string "foo", so it now
         * contains "packetfoo\0"
         */
         enet_packet_resize (packet, strlen ("packetfoo") + 1);
         strcpy (& packet -> data [strlen ("packet")], "foo");
         
         /* Send the packet to the peer over channel id 3.
         * One could also broadcast the packet by
         * enet_host_broadcast (host, 3, packet);
         */
         enet_peer_send (peer, 254, packet);
        /* One could just use enet_host_service() instead. */
        while (enet_host_service (client, & event, 3000) >= 0)
        {
            switch (event.type)
            {
              case ENET_EVENT_TYPE_RECEIVE:
                
                printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                    event.packet -> dataLength,
                    event.packet -> data,
                    event.peer -> data,
                    event.channelID);

                enet_packet_destroy (event.packet);
                break;
                
              case ENET_EVENT_TYPE_DISCONNECT:
                puts ("Disconnection succeeded.");
                return;
            }
        }
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was
         * received. Reset the peer in the event the 5 seconds
         * had run out without any significant event.
         */
        enet_peer_reset (peer);

        puts ("Connection to some.server.net:1234 failed.");
    }
    
    atexit (enet_deinitialize);
    return 0;
}
