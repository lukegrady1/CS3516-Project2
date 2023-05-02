#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"

//A
struct sender_window A_Window;

//calls from application and sends message to B
void A_output(struct msg message){

	if (Queue_full(A_Window.unacked_packets)) {
		//buffer the packets
		if (Queue_full(A_Window.buffered_messages)) {
			printf("Droping a message, buffer full \n");
		}
		else {
			printf("Window is full, adding to message buffer \n");
			struct msg* message_ptr = (struct msg*) malloc(sizeof(struct msg));
			memcpy(message_ptr, &message, sizeof(struct msg));
			enqueue(A_Window.buffered_messages,message_ptr);
		}

	}

	else {
		//send the message if window not full
		printf("A window is not full, sending a message \n");
		send_message(&message);
	}
}

/*
 * A_input(packet), where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the B-side (i.e., as a result
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side.
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet){
	printf("A has received a pcket. \n");
	//check if the packet is not corrupt
	if (check_checksum(&packet)) {
		//if not corrupt, check if the message was an ACK
		if (packet.acknum == 1 && Queue_size(A_Window.unacked_packets) > 0) {
			printf("A received an ACK \n");

			//the packet is an ack, and there are unacked packets.
			int ack_seq_num = packet.seqnum;

			printf("Getting unacked packets \n");
				int i;
			for (i=0;i<Queue_size(A_Window.unacked_packets);i++) {
				struct pkt* elt_head = get_first_element(A_Window.unacked_packets);
        struct pkt head = *elt_head;
				printf("unacked packet: %d\n", head.seqnum );
				if (elt_head->seqnum <= ack_seq_num) {
					dequeue(A_Window.unacked_packets); //removes unacked packet if less or equal to ack number
					free(elt_head); 
				}
				else {
					printf("Sequence number is higher, there are no packets to ack. \n");
					//seqnum is higher than the packet
					break;
				}
			}

			if (Queue_size(A_Window.unacked_packets) == 0) {
				//if no unacked packets left stop the timer
				printf("all packets ack'd, stopping the timer \n");
				stopTimer(AEntity);
			}

			printf("Sending any messages in the buffer, that fit in window \n");
			while (!Queue_full(A_Window.unacked_packets)&& Queue_size(A_Window.buffered_messages) > 0) {
				struct msg* message = dequeue(A_Window.buffered_messages);
				send_message(message); // sends a message in the buffered queue
			}
		}
	}
	else {
		//if a corrupt packet is found
		printf("Corrupt packet found\n");
	}
}

/*
 * A_timerinterrupt()  This routine will be called when A's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See starttimer()
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
 void A_timerinterrupt(){
	printf("Timer Interrupted \n");
	//when timer goes then we resend all of the packets that are unacked
	if (Queue_size(A_Window.unacked_packets) > 0) {
		printf("resending unacked packets \n");

		void** list_elt = list_all(A_Window.unacked_packets);
		void** list_head = list_elt;

		int i;
		for (i=0;i<Queue_size(A_Window.unacked_packets);i++) {
			printf("A is sending an unacked packet to B \n");
			tolayer3(AEntity, *((struct pkt*) *list_head));
			list_head++;
		}

		free(list_elt);

		startTimer(AEntity, timeout);
	}
	else {
		printf("All packets already ack'd \n");
	}
}

void A_init(){
  A_Window.next_seq_num = 0; //sends the next num
  A_Window.window_size = 1; 
  A_Window.buffered_messages = create_list(50); 
  A_Window.unacked_packets = create_list(1);
}