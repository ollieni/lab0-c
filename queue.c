#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_q;
    new_q = malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(new_q);
    return new_q;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    struct list_head *pos, *temp;

    list_for_each_safe (pos, temp, l) {
        element_t *entry = list_entry(pos, element_t, list);
        list_del(pos);  // Unlink the element from the list
        test_free(entry->value);
        test_free(entry);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *new = malloc(sizeof(element_t));
    new->value = malloc(strlen(s) + 1);
    int len = strlen(s);
    strncpy(new->value, s, len + 1);
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *new = malloc(sizeof(element_t));
    new->value = malloc(strlen(s) + 1);
    int len = strlen(s);
    strncpy(new->value, s, len + 1);
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    element_t *remove_node = list_entry(head->next, element_t, list);
    strncpy(sp, remove_node->value, bufsize);
    list_del(&remove_node->list);
    return remove_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    element_t *remove_node = list_entry(head->prev, element_t, list);
    strncpy(sp, remove_node->value, bufsize);
    list_del(&remove_node->list);
    return remove_node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    struct list_head *delete_node = head->next;
    int i = 0, size = q_size(head);
    while (i < size / 2) {
        delete_node = delete_node->next;
        i++;
    }
    list_del(delete_node);
    element_t *entry = list_entry(delete_node, element_t, list);
    test_free(entry->value);
    test_free(entry);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    struct list_head *pos, *temp;
    int count = 0;
    list_for_each_safe (pos, temp, head) {
        if (pos->next == head) {
            if (count) {
                list_del(pos);
                element_t *entry = list_entry(pos, element_t, list);
                test_free(entry->value);
                test_free(entry);
            }
            break;
        }
        if (strcmp(list_entry(pos->next, element_t, list)->value,
                   list_entry(pos, element_t, list)->value) == 0) {
            list_del(pos);
            element_t *entry = list_entry(pos, element_t, list);
            test_free(entry->value);
            test_free(entry);
            count++;
        } else {
            if (count) {
                list_del(pos);
                element_t *entry = list_entry(pos, element_t, list);
                test_free(entry->value);
                test_free(entry);
            }
            count = 0;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    struct list_head *cur = head->next->next, *prev = head->next;
    int size = q_size(head), i = 0;
    while (i <= size - 2) {
        element_t *cur_element = list_entry(cur, element_t, list);
        element_t *prev_element = list_entry(prev, element_t, list);
        char *temp = cur_element->value;
        cur_element->value = prev_element->value;
        prev_element->value = temp;
        cur = cur->next->next;
        prev = prev->next->next;
        i = i + 2;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (q_size(head) == 0) {
        return;
    }
    struct list_head *pos, *temp, *new;
    list_for_each_safe (pos, temp, head) {
        new = pos->next;
        pos->next = pos->prev;
        pos->prev = new;
    }
    new = head->next;
    head->next = head->prev;
    head->prev = new;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (q_size(head) == 1 || list_empty(head) || k > q_size(head)) {
        return;
    }
    if (k == q_size(head)) {
        q_reverse(head);
        return;
    }
    struct list_head *current = head->next;
    LIST_HEAD(temp);
    LIST_HEAD(result);
    int count1 = 0, count2 = 0;
    int times = q_size(head) / k;
    while (count1 < times) {
        while (count2 < k) {
            current = current->next;
            count2++;
        }
        list_cut_position(&temp, head, current->prev);
        q_reverse(&temp);
        list_splice_tail_init(&temp, &result);
        count2 = 0;
        count1++;
    }
    list_splice_init(&result, head);
}

/* Find the tail ofthe list */
struct list_head *find_tail(struct list_head *head)
{
    struct list_head *current = head;
    if (current->next != head) {
        current = current->next;
    }
    return current;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    struct list_head *node, *safe;
    char *right_max = list_entry(head->prev, element_t, list)->value;
    for (node = (head)->prev, safe = node->prev; node != (head);
         node = safe, safe = node->prev) {
        element_t *entry = list_entry(node, element_t, list);
        if (strcmp(right_max, entry->value) >= 0) {
            right_max = entry->value;
        } else {
            list_del(node);
            test_free(entry->value);
            test_free(entry);
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    struct list_head *node, *safe;
    char *right_max = list_entry(head->prev, element_t, list)->value;
    for (node = (head)->prev, safe = node->prev; node != (head);
         node = safe, safe = node->prev) {
        element_t *entry = list_entry(node, element_t, list);
        if (strcmp(right_max, entry->value) <= 0) {
            right_max = entry->value;
        } else {
            list_del(node);
            test_free(entry->value);
            test_free(entry);
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
