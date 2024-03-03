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
    struct list_head *new = malloc(sizeof(struct list_head));
    if (!new)
        return NULL;

    INIT_LIST_HEAD(new);
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
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
    if (!head) {
        return false;
    }
    element_t *new = malloc(sizeof(element_t));
    if (!new) {
        return false;
    }
    new->value = malloc(strlen(s) + 1);
    if (!(new->value)) {
        free(new);
        return false;
    }
    int len = strlen(s);
    strncpy(new->value, s, len + 1);
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    }
    element_t *new = malloc(sizeof(element_t));
    if (!new) {
        return false;
    }
    new->value = malloc(strlen(s) + 1);
    if (!(new->value)) {
        free(new);
        return false;
    }
    int len = strlen(s);
    strncpy(new->value, s, len + 1);
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head)) {
        return NULL;
    }
    element_t *remove_node = list_entry(head->next, element_t, list);
    if (sp) {
        strncpy(sp, remove_node->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    list_del(&remove_node->list);
    return remove_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (list_empty(head)) {
        return NULL;
    }
    element_t *remove_node = list_entry(head->prev, element_t, list);
    if (sp) {
        strncpy(sp, remove_node->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
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

void print(struct list_head *head)
{
    struct list_head *cur = head;
    do {
        printf("%s => ", list_entry(cur, element_t, list)->value);
        cur = cur->next;
    } while (cur != head);
    printf("\n");
}

struct list_head *merge(struct list_head *l1, struct list_head *l2)
{
    if (l1 == l2) {
        return l1;
    }
    struct list_head *cur1, *cur2, *swap, result, head1, head2;
    INIT_LIST_HEAD(&result);
    INIT_LIST_HEAD(&head1);
    INIT_LIST_HEAD(&head2);
    list_add_tail(&head1, l1);
    list_add_tail(&head2, l2);
    cur1 = (&head1)->next;
    cur2 = (&head2)->next;

    while (cur1 != &head1 && cur2 != &head2) {
        if (strcmp(list_entry(cur1, element_t, list)->value,
                   list_entry(cur2, element_t, list)->value) <= 0) {
            swap = cur1;
            cur1 = cur1->next;
            list_del(swap);
            list_add_tail(swap, &result);
        } else {
            swap = cur2;
            cur2 = cur2->next;
            list_del(swap);
            list_add_tail(swap, &result);
        }
    }
    if (cur1 != &head1) {
        list_splice_tail(&head1, (&result));
    }
    if (cur2 != &head2) {
        list_splice_tail(&head2, (&result));
    }
    l1 = (&result)->next;
    (&result)->prev->next = l1;
    list_del(&result);
    return l1;
}

struct list_head *split_two(struct list_head *head)
{
    struct list_head *slow, *fast;
    slow = head;
    fast = head->next;

    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    return slow;
}

/* Sort elements of queue in ascending/descending order */
struct list_head *sort(struct list_head *head, bool descend)
{
    struct list_head *second_head, *first_tail, *mid;
    if ((head == NULL) || (head->next == head)) {
        return head;
    }

    mid = split_two(head);
    first_tail = mid;
    second_head = mid->next;
    first_tail->next = head;
    second_head->prev = head->prev;
    head->prev->next = second_head;
    head->prev = first_tail;

    struct list_head *result1 = sort(second_head, descend);
    struct list_head *result2 = sort(head, descend);

    head = merge(result1, result2);

    return head;
}

void q_sort(struct list_head *head, bool descend)
{
    if ((head == NULL) || q_size(head) <= 1) {
        return;
    }
    struct list_head *input = head->next;
    list_del_init(head);

    struct list_head *result = sort(input, descend);
    list_add_tail(head, result);
}

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

void merge_two_list(struct list_head *l1, struct list_head *l2)
{
    struct list_head *cur1 = l1->next, *cur2 = l2->next, *swap, result;
    INIT_LIST_HEAD(&result);
    while (cur1 != l1 && cur2 != l2) {
        if (strcmp(list_entry(cur1, element_t, list)->value,
                   list_entry(cur2, element_t, list)->value) <= 0) {
            swap = cur1;
            cur1 = cur1->next;
            list_del(swap);
            list_add_tail(swap, &result);
        } else {
            swap = cur2;
            cur2 = cur2->next;
            list_del(swap);
            list_add_tail(swap, &result);
        }
    }
    while (cur1 != l1) {
        swap = cur1;
        cur1 = cur1->next;
        list_del(swap);
        list_add_tail(swap, &result);
    }
    while (cur2 != l2) {
        swap = cur2;
        cur2 = cur2->next;
        list_del(swap);
        list_add_tail(swap, &result);
    }
    list_splice_tail(&result, l1);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    struct list_head *temp = head->next->next;
    queue_contex_t *context1 = list_entry(head->next, queue_contex_t, chain);
    int size = q_size(context1->q);
    while (temp != head) {
        queue_contex_t *cur = list_entry(temp, queue_contex_t, chain);
        size += q_size(cur->q);
        merge_two_list(context1->q, cur->q);
        temp = temp->next;
    }
    if (descend) {
        q_reverse(context1->q);
    }
    return size;
}
void q_shuffle(struct list_head *list)
{
    // length
    int length = q_size(list);

    // Perform Fisher-Yates shuffle
    for (int i = length - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        // Swap data of nodes at positions i and j
        struct list_head *node_i = list->next;
        struct list_head *node_j = list->next;

        for (int k = 0; k < i; k++) {
            node_i = node_i->next;
        }
        for (int k = 0; k < j; k++) {
            node_j = node_j->next;
        }

        char *temp = list_entry(node_i, element_t, list)->value;
        list_entry(node_i, element_t, list)->value =
            list_entry(node_j, element_t, list)->value;
        list_entry(node_j, element_t, list)->value = temp;
    }
}
