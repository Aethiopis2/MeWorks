//===============================================================================================================|
// neohack.c:
//  a program to show exchange of information between neo4j browser (Firefox or Chrome) and neo4j server and
//  dumps the messages exchanged between these two end-point; it's part of my attempt to figure out bolt-protocol
//
// Program Author:
//  Aethiopis II ben Zahab
//
// Date created:
//  27th of June 2022, Monday
//
// Last update:
//  27th of June 2022, Monday
//===============================================================================================================|

//===============================================================================================================|
// INCLUDES
//===============================================================================================================|
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>

//===============================================================================================================|
// GLOBALS
//===============================================================================================================|
const int BUFFER_SIZE = 65536;      // a 64K? that was a looooooooooot back in the day's wand't it?

//===============================================================================================================|
// PROTOTYPES
//===============================================================================================================|
void Dump_Hex(const char *ps_buffer, const size_t n_len);
void Dump_Char(const char *ps_buffer, const size_t n_len);
void *Run_Bolt(void *thread_id);

//===============================================================================================================|
// FUNCTIONS
//===============================================================================================================|
int main()
{
    int fd_neo, fd_listen, fd_browser;     // sockets
    int on = 1;     // socket options
    struct sockaddr_in cli_addr, serv_addr, temp_addr;
    socklen_t addr_len = sizeof(serv_addr);
    short listen_port = 7477;           // fake HTTP port for neo4j actually its 7474 for HTTP
    short cli_port = 7474;
    
    // start running server code
    fd_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_listen < 0)
    {
        perror("socket");
        return 0;
    } // end if
    
    memset(&serv_addr, 0, addr_len);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(listen_port);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    
    setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    if (bind(fd_listen, (struct sockaddr *)&serv_addr, addr_len) < 0)
    {
        perror("bind");
        close(fd_listen);
        return 0;
    } // end if
    
    if (listen(fd_listen, 5) < 0)
    {
        perror("listen");
        close(fd_listen);
        return 0;
    } // end if
    
    // start up the client connection; before entring the main loop
    if ( (fd_neo = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
        perror("client socket");
        close(fd_listen);
        return 0;
    } // end if client fail
    
    // fill up client informatio
    memset(&cli_addr, 0, addr_len);
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(cli_port);
    cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");      // let's save some typing time
    
    // now make a connection
    if ( connect(fd_neo, (struct sockaddr *)&cli_addr, addr_len) < 0)
    {
        perror("client connect");
        close(fd_neo);
        close(fd_listen);
        return 0;
    } // end if
    
    // create the bolt thread 
    pthread_t thread_id=1;
    int i = 1;
    pthread_create(&thread_id, NULL, Run_Bolt, &i);
    
    printf("Client connected to Neo4j HTTP port: %d\n", cli_port);
    printf("Now listening at port: %d\n", listen_port);
    
    for (;;)
    {
        fd_browser = accept(fd_listen, (struct sockaddr *)&temp_addr, &addr_len);
        int turn = 1;
        
        // client initalization
        //for (;;)
        {
            char buffer[BUFFER_SIZE+1];     // storage
            int bytes; 
            int r_bytes = 0;
            char *cont = (char *)"Content-Length: ";
            int content = 0;
            
            bytes = recv(fd_browser, buffer, BUFFER_SIZE, 0);
            if (bytes < 0)
            {
                perror("server recieve");
                close(fd_neo);
                close(fd_listen);
                close(fd_browser);
                return 0;
            } // end if
            
            //printf("Turn: %d\n", turn++);
            //printf("Browser ...\nsent: %d bytes\n\n",  bytes);
            //Dump_Char(buffer, bytes);
            //Dump_Hex(buffer, bytes);
            
            // send the buffer to client
            bytes = send(fd_neo, buffer, bytes, 0);
            if (bytes < 0)
            {
                perror("client send");
                close(fd_neo);
                close(fd_listen);
                close(fd_browser);
                return 0;
            } // end if
        
            // now recive responses from neo4j server
            do
            {
                memset(buffer, 0, sizeof(buffer));
                bytes = recv(fd_neo, buffer, BUFFER_SIZE, 0);
                if (bytes < 0)
                {
                    perror("client recieve");
                    close(fd_neo);
                    close(fd_listen);
                    close(fd_browser);
                    return 0;
                } // end if
                
                r_bytes += bytes;
                
                //printf("\nNeo4j Server ...\n");
                //printf("recieved: %d bytes\ntotal bytes thus far: %d\n\n", bytes, r_bytes);
                
                // parse out content only if we have to; lazy style
                if (content == 0)
                {
                    char *p = strstr(buffer, cont);
                    if (p)
                    {
                       char *q = strstr(p, "\r\n");
                       *q = '\0';
                       
                       p += strlen(cont);
                       content = atoi(p);
                       //printf("\n===> Content String: %s\n===> Content Length: %d", p, content);
                       *q = '\r';
                    } // end if p
                } // end if parsing content
                
                
                //Dump_Hex(buffer, bytes);
                
                // seek bolt actual address and replace with the bridge address; we want to
                //  see everything ...
                char *bolt1 = (char *)"neo4j://localhost:7687";
                char *bolt2 = (char *)"bolt://localhost:7687";
                buffer[BUFFER_SIZE + 1] = '\0';
                char *loc = strstr(buffer, bolt1);
                if (loc)
                {
                    loc += (strlen(bolt1) - 1);
                    *loc = '4';
                    
                    char *loc2 = strstr(loc, bolt2);
                    if (loc2)
                    {
                        loc2 += (strlen(bolt2) - 1);
                        *loc2 = '4'; 
                    } // end if replace 2
                } // end replace 1
                
                
                //Dump_Char(buffer, bytes);
                
                // finally send it back to browser
                bytes = send(fd_browser, buffer, bytes, 0);
                if (bytes < 0)
                {
                    perror("server send");
                    close(fd_neo);
                    close(fd_listen);
                    close(fd_browser);
                    return 0;
                } // end if
            
                
            } while (r_bytes < content);
            
        } // end nested forEver
        close(fd_browser);
    } // end forEver
    
    return 0;
} // end main

//==========================================================================================================|
/**
 * @brief 
 *  Prints the contents of buffer in hex notation much like hex viewer's do it.
 * 
 * @param ps_buffer 
 *  the information to dump as hex and char arrary treated as a char array.
 * @param len 
 *  the length of the buffer above
 */
void Dump_Hex(const char *ps_buffer, const size_t n_len)
{
    size_t i;       // loop var

    for (i = 0; i < n_len; i++)
    {
        if ( !(i % 8) )
            printf("\n\t%02X ", (uint8_t)ps_buffer[i]);
        else
            printf("%02X ", (uint8_t)ps_buffer[i]);
    } // end for

    printf("\n");
} // end Dump_Hex


//==========================================================================================================|
/**
 * @brief 
 *  Prints the contents of buffer in stright up character format.
 * 
 * @param ps_buffer 
 *  the information to dump as hex and char arrary treated as a char array.
 * @param len 
 *  the length of the buffer above
 */
void Dump_Char(const char *ps_buffer, const size_t n_len)
{
    size_t i;       // loop var

    printf("\n");
    for (i = 0; i < n_len; i++)
        printf("%c", ps_buffer[i]);

    printf("\n");
} // end Dump_Hex

//==========================================================================================================|
/**
 * pretends to be a neo4j server over its bolt port
 */
void *Run_Bolt(void *id)
{
    int fd_bolt, fd_listen, fd_browser;
    short bolt_port = 7687;
    short server_port = 7684;
    int on = 1;
    
    struct sockaddr_in bolt_addr, serv;
    int addr_len = sizeof(serv);
    
    // get a socket for fake bolt listening socket
    fd_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_listen < 0)
        pthread_exit(NULL);
    
    // fill up the socket
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(server_port);
    serv.sin_addr.s_addr = INADDR_ANY;
    
    setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    if (bind(fd_listen, (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        perror("bind for bolt");
        close(fd_listen);
        pthread_exit(NULL);
    } // end if
    
    if (listen(fd_listen, 5) < 0)
    {
        perror("bind for bolt");
        close(fd_listen);
        pthread_exit(NULL);
    } // end if
    
    // now connect with bolt
    fd_bolt = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_bolt < 0)
    {
        perror("socket to bolt");
        close(fd_listen);
        pthread_exit(NULL);
    } // end if
    
    // fill up the bolt strut
    memset(&bolt_addr, 0, addr_len);
    bolt_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    bolt_addr.sin_family = AF_INET;
    bolt_addr.sin_port = htons(bolt_port);
    
    if (connect(fd_bolt, (struct sockaddr *)&bolt_addr, addr_len) < 0)
    {
        perror("connecting with bolt");
        close(fd_listen);
        close(fd_bolt);
        pthread_exit(NULL);
    } // end if
    
    printf("Connected with bolt on port: %d\n", bolt_port);
    printf("BoltBridge listening on port: %d\n", server_port);
    
    for (;;)
    {
        struct sockaddr_in addr_in;
        fd_browser = accept(fd_listen, (struct sockaddr *)&addr_in, &addr_len);
        int turn = 1;
        
        if (fd_browser < 0)
        {
            perror("accepting from client on bolt bridge");
            close(fd_bolt);
            close(fd_listen);
            pthread_exit(NULL);
        } // end if
        
        for (;;)
        {
            char buffer[BUFFER_SIZE];     // storage
            int bytes; 
            int r_bytes = 0;
            char *cont = (char *)"Content-Length: ";
            int content = 0;

            // get from browser
            bytes = recv(fd_browser, buffer, BUFFER_SIZE, 0);
            if (bytes < 0)
            {
                perror("bolt bridge recieve");
                close(fd_bolt);
                close(fd_listen);
                close(fd_browser);
                pthread_exit(NULL);
            } // end if
            
            printf("\nTurn: %d\n", turn++);
            printf("Browser for Bolt ...\nsent: %d bytes\n\n",  bytes);
            Dump_Char(buffer, bytes);
            Dump_Hex(buffer, bytes);
            
            // send to bolt
            bytes = send(fd_bolt, buffer, bytes, 0);
            if (bytes < 0)
            {
                perror("sending to bolt");
                close(fd_bolt);
                close(fd_listen);
                close(fd_browser);
                pthread_exit(NULL);
            } // end if
            
            // get back from bolt
            memset(buffer, 0, BUFFER_SIZE);
            bytes = recv(fd_bolt, buffer, BUFFER_SIZE, 0);
            if (bytes < 0)
            {
                perror("receiving from bolt");
                close(fd_bolt);
                close(fd_listen);
                close(fd_browser);
                pthread_exit(NULL);
            } // end if
            
            
            printf("Bolt ...\nreceived: %d bytes\n\n",  bytes);
            Dump_Char(buffer, bytes);
            Dump_Hex(buffer, bytes);
            
            // forward to browser
            bytes = send(fd_browser, buffer, bytes, 0);
            if (bytes < 0)
            {
                perror("sending to browser");
                close(fd_bolt);
                close(fd_listen);
                close(fd_browser);
                pthread_exit(NULL);
            } // end if
        } // end for
        
        close(fd_browser);
    } // end forEver
    
    pthread_exit(NULL);
} // end Run_Bolt

//===============================================================================================================|
//          THE END
//===============================================================================================================| 
