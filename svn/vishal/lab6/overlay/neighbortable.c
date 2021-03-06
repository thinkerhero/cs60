//FILE: overlay/neighbortable.c
//
//Description: this file the API for the neighbor table
//
//Date: May 03, 2010

#include "../topology/topology.h"
#include "neighbortable.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/utsname.h>
#include <assert.h>
/**
typedef struct neighborentry {
  int nodeID;	        //neighbor's node ID
  in_addr_t nodeIP;     //neighbor's IP address
  int conn;	        //TCP connection's socket descriptor to the neighbor
} nbr_entry_t;
*/
//This function first creates a neighbor table dynamically. It then parses the topology/topology.dat file and fill the nodeID and nodeIP fields in all the entries, initialize conn field as -1 .
//return the created neighbor table
nbr_entry_t* nt_create()
{	
	printf("nt_create called \n");
	nbr_entry_t* table = NULL;
	int size = 0;
	int myNodeId = topology_getMyNodeID();
	FILE *topologyFile = fopen(TOPOLOGY_FILE_NAME, "r");
	if(topologyFile != NULL){
		char line[120];
		while(fgets(line, sizeof(line), (FILE*)topologyFile) != NULL){
			char *firstHost = strtok(line, " ");
			char *secondHost = strtok(NULL, " ");
			int firstNodeId = topology_getNodeIDfromname(firstHost);
			int secondNodeId = topology_getNodeIDfromname(secondHost);
			if(myNodeId == firstNodeId || myNodeId == secondNodeId ){ // is neighbour
				char *neighbourHostName ;
				int neighbourId ;
				if(myNodeId == firstNodeId){
					neighbourHostName = secondHost;
					neighbourId = secondNodeId;
				}else{
					neighbourHostName = firstHost;
					neighbourId = firstNodeId;
				}
				//printf("Neighbour found  neighbour host = %s neighbourid = %d \n",neighbourHostName, neighbourId);
				nbr_entry_t *new_entry = (nbr_entry_t *)malloc(sizeof(nbr_entry_t)); ;
				new_entry->nodeID = neighbourId;
				new_entry->conn = -1;
				get_ip_from_host_name(neighbourHostName,&new_entry->nodeIP);
				if(size == 0){
					table = (nbr_entry_t *)realloc(table,sizeof(nbr_entry_t));
					table[size++] = *new_entry;
				}else { // new node is found
					table = (nbr_entry_t *)realloc(table, (size + 1) * sizeof(nbr_entry_t));
					table[size] = *new_entry;
					size++;
				}
			}
		}
		fclose(topologyFile);
	}
	printf("nt_create ended \n");
	return table;
}

//This function destroys a neighbortable. It closes all the connections and frees all the dynamically allocated memory.
void nt_destroy(nbr_entry_t* nt)
{	
	printf("nt_destroy called ..\n");
	int neighbors = topology_getNbrNum();
	printf("freeing neighbor table total = %d \n",neighbors);
	for (int i = 0; i < neighbors; i++) {
        printf("closing connection %d \n", nt[i].conn );
        close(nt[i].conn);
    }
    free(nt);    
}

//This function is used to assign a TCP connection to a neighbor table entry for a neighboring node. If the TCP connection is successfully assigned, return 1, otherwise return -1
int nt_addconn(nbr_entry_t* nt, int nodeID, int conn)
{	
	printf("nt_addconn started for node id  %d  and conn id %d \n", nodeID, conn);
	if(nt){
		printf("nt is ok\n");
	    int totalNeighbours = topology_getNbrNum(); 
	    for (int i = 0; i < totalNeighbours ; i++) {
	        if (nt[i].nodeID == nodeID) { // check for proper neighbour to be found 
	        	printf("node found id = %d\n",nodeID);
	            nt[i].conn = conn;
	           	printf("nt_addconn ending with success %d\n",nodeID);
	            return 1;
	        }
	    }
	}
	printf("nt_addconn ending with fail %d\n",nodeID);
  return -1;
}

int main5(){
	int neighbourCount = topology_getNbrNum();
	nbr_entry_t *neighbor = nt_create();
	for(int i = 0 ; i < neighbourCount ; i++){
		printf("Node id = %d Node ip %d con  = %d \n",neighbor[i].nodeID, topology_getNodeIDfromip((struct in_addr *)&neighbor[i].nodeIP), neighbor->conn) ;
	}
	return 0; 
}
