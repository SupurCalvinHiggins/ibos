typedef u8 IBOS_event_id;
typedef IBOS_memory_block IBOS_event_data;

typedef struct {
	IBOS_event_id event_id;
	IBOS_event_data event_data;
} IBOS_event;

typedef struct {
	IBOS_event* events;
	size_t capacity;
	size_t size;
} IBOS_event_queue;

IBOS_event_queue IBOS_event_queue_allocate(size_t capacity);
void IBOS_event_queue_deallocate(IBOS_event_queue event_queue);

void push(IBOS_event_queue event_queue, IBOS_event event);
void pop(IBOS_event_queue event_queue);

IBOS_event peek(IBOS_event_queue event_queue);
size_t size(IBOS_event_queue event_queue);
bool is_empty(IBOS_event_queue event_queue);
bool is_full(IBOS_event_queue event_queue);
