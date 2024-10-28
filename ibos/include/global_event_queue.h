

typedef struct global_event {
	thread_id_t thread_id;
	event_t event;	
} global_event_t;

typedef struct global_event_queue {
	global_event_t* global_events;
	size_t capacity;
	size_t size;
} global_event_queue_t;

global_event_queue_t global_event_queue;

global_event_queue_t init(size_t capacity);

void global_event_queue_lock(global_event_queue_t global_event_queue)
void global_event_queue_unlock(global_event_queue_t global_event_queue);

void push(global_event_queue_t event_queue, global_event_t event);
void pop(event_queue_t event_queue);

event_t peek(event_queue_t event_queue);
size_t size(event_queue_t event_queue);
bool is_empty(event_queue_t event_queue);
bool is_full(event_queue_t event_queue);
