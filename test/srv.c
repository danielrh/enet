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
    atexit (enet_deinitialize);

    
    ENetAddress address;
    ENetHost * server;

    /* Bind the server to the default localhost.
     * A specific host address can be specified by
     * enet_address_set_host (& address, "x.x.x.x");
     */
    address.host = ENET_HOST_ANY;
    /* Bind the server to port 1234. */
    address.port = 1234;

    server = enet_host_create (& address /* the address to bind the server host to */, 
                               32767 /* allow up to 32 clients and/or outgoing connections */,
                0 /* assume any amount of incoming bandwidth */,
                0 /* assume any amount of outgoing bandwidth */);
    if (server == NULL)
    {
        fprintf (stderr, 
                 "An error occurred while trying to create an ENet server host.\n");
        exit (EXIT_FAILURE);
    }
    ENetEvent event;
    
    /* Wait up to 1000 milliseconds for an event. */
    while (enet_host_service (server, & event, 1000) >=0)
    {
        if (event.type){
            printf ("%d\n",event.type);
        }
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            printf ("A new client connected from %x:%u.\n", 
                    event.peer -> address.host,
                    event.peer -> address.port);

            /* Store any relevant client information here. */
            event.peer -> data = "Client information";
            {
                ENetPacket * packet = enet_packet_create ("packet", 
                                                          strlen ("packet") + 1, 
                                                          ENET_PACKET_FLAG_RELIABLE);
                
                /* Extend the packet so and append the string "foo", so it now
                 * contains "packetfoo\0"
                 */
                enet_packet_resize (packet, strlen ("packetbar") + 1);
                strcpy (& packet -> data [strlen ("packet")], "bar");
                
                enet_peer_send (event.peer, 3, packet);
            }
            continue;

        case ENET_EVENT_TYPE_RECEIVE:
            printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
                    event.packet -> dataLength,
                    event.packet -> data,
                    event.peer -> data,
                    event.channelID);

            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy (event.packet);
            
            continue;
           
        case ENET_EVENT_TYPE_DISCONNECT:
            printf ("%s disconected.\n", event.peer -> data);

            /* Reset the peer's client information. */

            event.peer -> data = NULL;
        }
    } 
    enet_host_destroy(server);
    return 0;
}
