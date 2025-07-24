// MIT License

// Copyright (c) 2025 Rajan Gupta

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <signal.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "linked_list.h"
#include "queue.h"

#define TEST(x)                                                                \
    printf("Running test " #x "\n");                                           \
    fflush(stdout);
#define SUBTEST(x)                                                             \
    printf("    Executing subtest " #x "\n");                                  \
    fflush(stdout);                                                            \
    alarm(1);
#define FAIL(cond, msg)                                                        \
    if (cond)                                                                  \
    {                                                                          \
        printf("    FAIL! ");                                                  \
        printf(#msg "\n");                                                     \
        exit(-1);                                                              \
    }
#define PASS(x)                                                                \
    printf("PASS!\n");                                                         \
    alarm(0);

bool instrumented_malloc_fail_next = false;
bool instrumented_malloc_last_alloc_successful = false;

void gracefully_exit_on_suspected_infinite_loop(int signal_number)
{
    // Use write() to tell the tester that they're probably stuck
    // in an infinite loop.
    //
    // Why not printf()/fprintf()? It goes against POSIX rules for
    // signal handlers to call a non-reentrant function, of which both
    // of those are. I had no about that constraint prior to writing
    // this function. Cool!
    //
    const char* err_msg = "        Likely stuck in infinite loop! Exiting.\n";
    ssize_t retval = write(STDOUT_FILENO, err_msg, strlen(err_msg));
    fflush(stdout);

    // We really don't care about whether write() succeeded or failed
    // or whether a partial write occurred. Further, we only install
    // this function to one signal handler, so we can ignore that as well.
    //
    (void)retval;
    (void)signal_number;

    // Exit.
    //
    exit(1);
}

// A wrapper around malloc() that lets the test code force
// it to fail.
//
void* instrumented_malloc(size_t size)
{
    if (instrumented_malloc_fail_next)
    {
        instrumented_malloc_fail_next = false;
        instrumented_malloc_last_alloc_successful = false;
        return NULL;
    }

    void* ptr = malloc(size);
    instrumented_malloc_last_alloc_successful = (ptr != NULL);

    return ptr;
}

void check_empty_list_properties()
{
    TEST(check_empty_list_properties)
    SUBTEST(linked_list_create)
    linked_list* ll = new linked_list();

    // Check that size returns 0.
    //
    FAIL(
        ll->size() != 0,
        "linked_list::size() returns non-zero for an empty list.");

    SUBTEST(linked_list_remove)
    // FAIL(ll->remove(0) != false, "linked_list::remove() returned true for
    // empty list on index 0.");

    delete ll;
    PASS(check_empty_list_properties)

    TEST(check_empty_queue_properties)
    queue* q = new queue();

    SUBTEST(queue_size)
    FAIL(q->size() != 0, "queue::size() returned non-zero on empty queue");

    SUBTEST(queue_has_next)
    FAIL(
        q->has_next() != false,
        "queue::has_next() returned true on empty queue");

    unsigned int popped_data;

    SUBTEST(queue_pop)
    FAIL(
        q->pop(&popped_data) != false,
        "queue::pop() returned true on empty queue");

    delete q;
    PASS(check_empty_queue_properties)
}

void check_insertion_functionality()
{
    TEST(check_insertion_functionality)
    SUBTEST(check_insert_end)
    // Check insertion at end by manually checking each element.
    // Inserts 1, 2, 3, 4 into the list, verifies data.
    //
    linked_list* ll = new linked_list();
    size_t ll_size = SIZE_MAX;
    FAIL(ll == NULL, "Failed to create new linked_list (#1)")
    for (size_t i = 1; i <= 4; i++)
    {
        bool status = ll->insert_end(i);
        FAIL(status == false, "Failed to insert node into linked_list #1")
    }

    SUBTEST(iterate_over_linked_list_1)
    for (size_t i = 1; i <= 4; i++)
    {
        FAIL(
            (*ll)[i - 1] != i, "Linked list (#1) does not contain correct data")
    }
    delete ll;

    // Check insertion at front.
    // Inserts 4, 3, 2, 1 into the list, verifies data.
    //
    SUBTEST(check_insert_front)
    ll = new linked_list();
    ll_size = ll->size();
    FAIL(ll_size != 0, "linked_list (#2) size is non-zero when created")
    FAIL(ll == NULL, "Failed to create new linked_list (#2)")
    for (size_t i = 4; i != 0; i--)
    {
        bool status = ll->insert_front(i);
        FAIL(status == false, "Failed to insert node into linked_list #2")
    }

    ll_size = ll->size();
    FAIL(ll_size != 4, "linked_list (#2) size was not equal to 4")

    SUBTEST(iterate_over_linked_list_2)
    for (size_t i = 1; i <= 4; i++)
    {
        FAIL(
            (*ll)[i - 1] != i, "Linked list (#2) does not contain correct data")
    }
    ll_size = ll->size();
    FAIL(ll_size != 4, "linked_list (#2) size was not equal to 4")
    delete ll;

    SUBTEST(check_insert_function_at_end)
    // Checck that insertion of a single element at an out of bounds
    // index works on an empty linked list.
    //
    ll = new linked_list();
    ll_size = ll->size();
    FAIL(ll_size != 0, "linked_list (#3) size is non-zero when created")
    FAIL(ll == NULL, "Failed to create new linked_list (#3)")

    FAIL(
        ll->insert(1, 0) != false,
        "Inserted index in empty linked_list (#3) at index 1.")

    // Check insertion at end, same 1-4 test.
    //
    for (size_t i = 0; i < 4; i++)
    {
        bool status = ll->insert(i, i + 1);
        FAIL(
            status != true,
            "ll->insert() failed for linked_list (#3) unexpectedly")
    }

    for (size_t i = 1; i <= 4; i++)
    {
        FAIL(
            (*ll)[i - 1] != i,
            "Iterator does not contain correct data for linked_list (#3)")
    }
    delete ll;

    SUBTEST(check_insert_function_at_beginning)
    // Checks insertion at the beginning, same 4-1 test, but using
    // the insert() function instead of insert_front().
    //
    ll = new linked_list();
    for (size_t i = 4; i != 0; i--)
    {
        bool status = ll->insert(0, i);
        FAIL(
            status != true,
            "ll->insert() failed for linked_list (#4) unexpectedly");
    }

    for (size_t i = 1; i <= 4; i++)
    {
        FAIL(
            (*ll)[i - 1] != i, "Linked list (#4) does not contain correct data")
    }

    delete ll;

    SUBTEST(check_insert_function_at_middle);
    ll = new linked_list();
    bool status = ll->insert(0, 1);
    FAIL(
        status == false,
        "Failed to insert 1 at the beginning of linked_list (#5)")
    status = ll->insert(1, 2);
    FAIL(status == false, "Failed to insert 2 at the end of linked_list (#5)")
    status = ll->insert(2, 4);
    FAIL(status == false, "Failed to insert 4 at the end of linked_list (#5)")

    status = ll->insert(2, 3);
    FAIL(
        status == false, "Failed to insert 3 at the middle of linked_list (#5)")

    // Standard 1-4 check.
    //
    for (size_t i = 1; i <= 4; i++)
    {
        FAIL(
            (*ll)[i - 1] != i,
            "Iterator does not contain correct data for linked_list (#3)")
    }

    delete ll;

    PASS(check_insertion_functionality)

    TEST(check_queue_insertion_functionality)
    queue* q = new queue();

    SUBTEST(queue_push)
    // Insert 1-5, check various things.
    //
    for (size_t i = 1; i <= 5; i++)
    {
        bool status = q->push(i);
        FAIL(status != true, "queue::push() did not return TRUE")
    }

    SUBTEST(queue_size)
    FAIL(q->size() != 5, "queue::size() did not return 5")

    SUBTEST(queue_has_next)
    FAIL(q->has_next() != true, "queue::has_next() did not return true")

    SUBTEST(queue_next_value)
    unsigned int next_value;
    status = q->next(&next_value);
    FAIL(status != true, "queue::next() did not return true")
    FAIL(next_value != 1, "queue::next() did not return next value of 1")

    PASS(check_queue_insertion_functionality)

    TEST(queue_pop_values)
    SUBTEST(pop_values)
    for (size_t i = 1; i <= 5; i++)
    {
        unsigned int value = 20;
        bool status = q->pop(&value);

        FAIL(status != true, "queue::pop() did not return TRUE")
        FAIL(value != i, "queue::pop() popped data is not correct")
    }

    SUBTEST(queue_size)
    FAIL(q->size() != 0, "queue::size() is not zero after all elements popped")

    delete q;
    PASS(queue_pop_values)
}

void check_find_functionality(void)
{
    TEST(check_find_functionality)

    // Single linked_list, find elements.
    //
    linked_list* ll = new linked_list();

    // Create list of ints 1 to 10.
    //
    for (size_t i = 0; i < 10; i++)
    {
        ll->insert_end(i + 1);
    }

    // Find beginning.
    //
    SUBTEST(find_beginning)
    size_t index = ll->find(1);
    FAIL(index != 0, "Did not find 1 at beginning of linked_list")

    // Find end.
    //
    SUBTEST(find_end)
    index = ll->find(10);
    FAIL(index != 9, "Did not find 10 at end of linked_list")

    // Find middle.
    //
    SUBTEST(find_middle)
    index = ll->find(5);
    FAIL(index != 4, "Did not find 5 at end of linked_list")

    // Find non-existant data
    //
    SUBTEST(find_nonexistant)
    index = ll->find(11);
    FAIL(index != SIZE_MAX, "Found 11 when it is not in the linked_list")

    ll->insert_end(6);

    // Ensure the first instance of 6 is found.
    //
    SUBTEST(find_first_of_duplicate_data)
    index = ll->find(6);
    FAIL(
        index != 5,
        "Failed to find 6, when duplicate added to end of linked_list")

    delete ll;

    PASS(check_find_functionality)
}

int main(void)
{
    // Set up signal handler for catching infinite loops.
    //
    signal(SIGALRM, gracefully_exit_on_suspected_infinite_loop);

    // Set up instrumented malloc and free
    //
    linked_list::register_malloc(instrumented_malloc);
    linked_list::register_free(free);
    queue::register_malloc(instrumented_malloc);
    queue::register_free(free);

    // Various checks.
    //
    check_empty_list_properties();
    check_insertion_functionality();
    check_find_functionality();

    return 0;
}
