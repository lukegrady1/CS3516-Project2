/*
 * This is the include file that contains definitions of various structures.
 * It also contains prototypes needed by the routines in the student code.
 */

/* ***************************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR:
   This code should be used for unidirectional or bidirectional
   data transfer protocols from A to B and B to A.
   Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets may be delivered out of order
 VERSION 1.1  J.F.Kurose
 VERSION 2.0  J. Breecher  - October  2005
 VERSION 2.05 J. Breecher  - November 2005
 VERSION 2.10 J. Breecher  - February 2011
 VERSION 2.20 J. Breecher  - October  2013
*****************************************************************************/
#define   LINUX
//#define    WINDOWS

#define  TRUE            1
#define  FALSE           0
/*  This defines which side (entity) is making a call to the simulator */
#define  AEntity         0
#define  BEntity         1

//timeout limit
#define timeout 50.0

// Sender A
struct sender_window A_Window; //has to be set here


// A "msg" is the data unit passed from layer 5 (Application code) to layer
// 4 (student's code).  It contains the data (characters) to be delivered
// to the remote layer 5 via the students transport level protocol entities.

#define  MESSAGE_LENGTH  20
struct msg {
    char  data[MESSAGE_LENGTH];
};

// A packet is the data unit passed from layer 4 (student's Transport Layer
// code) to layer 3 (the network simulator).  Note this is the pre-defined
// packet structure, that is expected by layer 3 - so student code must
// match this format.
struct   pkt {
    int  seqnum;
    int  acknum;
    int  checksum;
    char payload[MESSAGE_LENGTH];
};


//Message window struct for the Go-Back-N implementation
struct sender_window {
	int next_seq_num;	//next sequence num
	int max_seq_num; //max sequence num
	int window_size; //window size N
	struct list* buffered_messages; //whole set of unacked and unsent
	struct list* unacked_packets; //set of unacked messages
};

//reciever window implimentation
struct receiver_window {
	int expected_seq_num; //expected sequence number
	struct pkt* last_ack; //pointer to the last ack message sent.
};

//QUEUE based struct for the Go-Back-N implementation
struct list {
	void** values; // pointer to an array of pointers to values
	int curr_size; // the amount of elements currently in the list
	int max_size; // the maximum size of the list
	void** head; // pointer to the current head
	void** tail; // the pointer to the first free spot in the list.
};


/*
 * PROTOTYPES - These are in student.c
 * */

void A_output(struct msg message);  // A-Side App call to A-side transport
void B_output(struct msg message);  // B-Side App call to B-side transport
void A_input(struct pkt packet);    // From network layer UP to A transport
void A_timerinterrupt();            // A-side code that is called on interrupt
void A_init();                      // Simulator calls this at initialization
void B_input(struct pkt packet);    // From network layer UP to A transport
void B_timerinterrupt();            // B-side code that is called on interrupt
void B_init();                      // Simulator calls this at initialization

/*
 * PROTOTYPES - These are in Project3.c
 *
 * These are routines that must be visible from student.c so that the code
 * written by students can get to these routines.
 * */

/*
 * format:  startTimer(calling_entity, increment),
 * 1st argument: calling_entity is either AENTITY or BENTITY
 * 2nd argument: increment is a double value indicating the amount of time
 * that will pass before the timer interrupts.
 * A's timer should only be started (or stopped) by A-side routines,
 * and similarly for the B-side timer.
 */
void startTimer(int AorB, double increment);

/*
 * stopTimer(calling_entity),
 * 1st argument: calling_entity is either AENTITY or BENTITY
 */
void stopTimer( int AorB );

/*
 * getClockTime()
 * Get the current simulation time - note that the call returns a double
 *   representing that time - it's not the time that's on your real clock.
 */
double  getClockTime( );

/*
 * tolayer3( calling_entity, packet).
 * 1st argument: calling_entity is either AENTITY or BENTITY
 * 2nd argument: packet is a structure of type pkt.
 * Calling this routine will cause the packet to be sent into the network,
 * destined for the other entity.
 */
void tolayer3( int  AorB, struct pkt packet );

/*
 * Send message from your transport later to the application layer.
 * tolayer5( calling_entity, message).
 * 1st argument: calling_entity is either AENTITY or BENTITY
 * 2nd argument: message is a structure of type msg.
 *
 */
void tolayer5( int AorB, struct msg datasent);

/*
 * getTimerStatus( calling_entity),
 * 1st argument: calling_entity is either AENTITY or BENTITY
 *
 * Returns the current state of the timer - TRUE if running, FALSE if not
 */
int getTimerStatus( int AorB );


//Custom functions comments by implementation
void get_next_seqnum(struct sender_window* window);

void update_receiver_seqnum(struct receiver_window* window);

char *int_binary(int a, char *buffer, int buf_size);

int make_checksum(struct pkt* packet);

int check_checksum(struct pkt* packet);

struct pkt* set_ACK(int seq_num);

void** list_all(struct  list* list);

int Queue_full(struct list* list);

int Queue_size(struct list* list);

void* get_first_element(struct list* list);

void* dequeue(struct list* list);

int enqueue(struct list* list, void* value);

void send_message(struct msg* message);

void send_application_layer(struct pkt* packet);

struct list* create_list(int size);

void free_list(struct list* list);