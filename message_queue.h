#ifndef __MSG_QUEUE_H
#define __MSG_QUEUE_H

#include "message.h"

/* message node */

typedef struct __msg_node {
	message *msg;
	struct __msg_node *next;
} msg_node;

msg_node* msg_node_destroy(msg_node *node)
{
	msg_node *next = NULL;
	if (node) {
		next = node->next;
		if (node->msg)
			free(node->msg);
		free(node);
	}

	return next;
}

void msg_node_init(msg_node *node, message *msg, msg_node *next)
{
	node->msg = msg;
	node->next = next;
}

/* message queue */

typedef struct __msg_queue {
	msg_node *head;
	msg_node *tail;
} msg_queue;

void msg_queue_init(msg_queue *queue)
{
	memset(queue, 0, sizeof(msg_queue));
}

void msg_queue_destroy(msg_queue *queue)
{
	msg_node *curr = queue->head;
	while(curr = msg_node_destroy(curr), curr != NULL);
}

void msg_queue_push(msg_queue *queue, message *msg)
{
	msg_node *node = malloc(sizeof(msg_node));
	msg_node_init(node, msg, NULL);

	if (queue->head == NULL) {
		queue->head = node;
		queue->tail = node;
	}
	else {
		queue->tail->next = node;
		queue->tail = node;
	}
}

message* msg_queue_pop(msg_queue *queue)
{
	message *msg = queue->head->msg;
	msg_node *poped = queue->head;
	queue->head = queue->head->next;
	free(poped);
	return msg;
}

int msg_queue_is_empty(msg_queue *queue)
{
	return (queue->head == NULL);
}

#endif