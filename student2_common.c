#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"



//sequence number ++ by 1
void get_next_seqnum(struct sender_window* window) {
	window->next_seq_num ++;
}

//receiver number updated
void update_receiver_seqnum(struct receiver_window* window) {
	window->expected_seq_num ++;
}

#define BUF_SIZE 37 

//print int to binary 
char *int_binary(int a, char *buffer, int buf_size) {
    buffer += (buf_size - 1);
	int i;
    for (i = 31; i >= 0; i--) {
        *buffer-- = (a & 1) + '0';
        a >>= 1;
    }
    return buffer;
}


//checksum
int make_checksum(struct pkt* packet) {
	int checksum = 0;
	checksum += packet->seqnum;
	checksum += packet->acknum;
  //sum 
	int i;
	for (i=0;i<20;i++) {
		checksum += ((int) packet->payload[i] * i); // multiply row by index and take the sum
	}

  printf("Setting the CheckSum = %d\n", ~checksum);
	return ~checksum;
}

//0 if good 1 if not
int check_checksum(struct pkt* packet) {
	int res = 0;
	int checksum = make_checksum(packet);
	printf("Getting the CheckSum = %d\n",packet->checksum);
	if(checksum == packet->checksum){
		return 1;
	}else{
		return 0;
	}
}

//set the packet as ACK
struct pkt* set_ACK(int seq_num) {
	struct pkt* packet = (struct pkt*) malloc(sizeof(struct pkt));
	memset(packet, 0, sizeof(struct pkt)); //struct set to zero
	packet->seqnum = seq_num;
	packet->acknum = 1;
	packet->checksum = make_checksum(packet);
	return packet;
}


//gives all the values that are in a given queue
void** list_all(struct list* list) {
	if (list->curr_size == 0) {
		return NULL; //if there are no elements in the list
	}
	void** values = (void**) malloc ( sizeof(void*) * list->curr_size);
	void** values_tmp = values;
	void** tmp_head = list->head;
		int i;
	for (i=0;i<list->curr_size;i++) {
		*(values_tmp) = *(tmp_head);
		values_tmp++;
		tmp_head++;
		if (tmp_head>= (list->values + list->max_size)) {
			tmp_head = list->values;
		}
	}
	return values;
}

//checks if the list is full
int Queue_full(struct list* list) {
	return list->curr_size == list->max_size;
}

//gets the size of the list
int Queue_size(struct list* list) {
	return list->curr_size;
}

//gets the first element that is in the queue
void* get_first_element(struct list* list) {
	return *(list->head);
}

void* dequeue(struct list* list) {
	void* val = NULL;
	if (list->curr_size > 0) {	//checks if the list is not empty
		val = *(list->head);
		list->head++;

		if (list->head >= (list->values + list->max_size)) {
			list->head = list->values;
		}
		list->curr_size--; //-- the size currently
	}
	return val;
}
//add queue
int enqueue(struct list* list, void* value) {
	if (!Queue_full(list)) {
		*(list->tail) = value;
		list->tail++; 
		list->curr_size++;
		if (list->tail >= (list->values + list->max_size)) {
			list->tail = list->values;
		}
	}
}
//send to layer 3
void send_message(struct msg* message) {

	struct pkt* packet = (struct pkt*) malloc (sizeof(struct pkt));
	packet->seqnum = A_Window.next_seq_num; 
	get_next_seqnum(&A_Window);
	packet->acknum = 0; 
	memcpy(packet->payload, message->data, 20); 
	// create checksum
	packet->checksum = make_checksum(packet);

	enqueue(A_Window.unacked_packets, packet); //adds packet to unacked
	tolayer3(AEntity, *packet);

	//starts timer no unacked packets
	if (Queue_size(A_Window.unacked_packets) == 1) {
		startTimer(AEntity, timeout);
	}
}

//send layer 5
void send_application_layer(struct pkt* packet) {
	struct msg message; 
	strncpy(message.data, packet->payload, 20);
	tolayer5(BEntity,message); 
}

//creates list with the number of total messages
struct list* create_list(int size) {
	struct list* list = (struct list*) malloc (sizeof(struct list));
	list->values = (void**) malloc( sizeof(void*) * size);
	list->max_size = size;
	list->curr_size = 0;
	list->head = list->values;
	list->tail = list->values;
	return list;
}

void free_list(struct list* list) {
	free(list->head);
	free(list);
}