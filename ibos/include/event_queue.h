#pragma once

#include "int.h"
#include "memory.h"
#include "port.h"

typedef u8 IBOS_event_id_t;
typedef IBOS_memory_block_t IBOS_event_data_t;

typedef struct {
  IBOS_event_id_t id;
  IBOS_event_data_t data;
} IBOS_event_t;

typedef struct {
  IBOS_event_t *base;
  usize start;
  usize end;
  usize capacity;
} IBOS_event_queue_t;

IBOS_event_queue_t IBOS_event_queue_allocate(usize capacity);
void IBOS_event_queue_deallocate(IBOS_event_queue_t *queue);

void IBOS_event_queue_push(IBOS_event_queue_t *queue, IBOS_event_t event);
void IBOS_event_queue_pop(IBOS_event_queue_t *queue);

IBOS_event_t IBOS_event_queue_peek(IBOS_event_queue_t queue);
usize IBOS_event_queue_size(IBOS_event_queue_t queue);
usize IBOS_event_queue_capacity(IBOS_event_queue_t queue);
