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

#include "queue.h"
#include <cassert>

// Initial declaration of the static member function
// pointers in the linked_list class.
//
void* (*queue::malloc_fptr)(size_t) = nullptr;
void (*queue::free_fptr)(void*) = nullptr;

// TODO - explore using a memory pool/allocator

// Constructor
queue::queue() : ll{new linked_list()}
{
    if (ll == nullptr)
    {
        throw std::bad_alloc();
    }
};

// Destructor
queue::~queue() { delete ll; };

// TODO - comment
void* queue::operator new(size_t size)
{
    assert(malloc_fptr);
    return malloc_fptr(size);
};

// TODO - comment
void queue::operator delete(void* ptr)
{
    assert(free_fptr);
    free_fptr(ptr);
};

// TODO
// Adds a new node to the end of the queue
bool queue::push(unsigned int data)
{
    assert(ll);
    return ll->insert_end(data);
};

// TODO
// Removes the first node from the queue, the value of the node is placed in
// popped_data, returns true on success, false otherwise
// Does not modify popped_data if pop() fails.
bool queue::pop(unsigned int* popped_data)
{
    // We could check if the queue is empty here, but for now I'm happy to let
    // ll->remove() handle edge cases in order to keep this function simple.

    assert(ll);

    // Could be [[unlikely]] assuming we're mainly popping from non-empty
    // queues, really depends on the use case - TODO
    if (!has_next())
    {
        return false;
    }

    auto tmp = ll->operator[](0);

    // This should only happen if we have a bug in our code
    if (!ll->remove(0)) [[unlikely]]
    {
        return false;
    }

    *popped_data = tmp;
    return true;
};

// TODO
// Checks if the queue has any more nodes
// IE is the queue NOT empty
bool queue::has_next() const
{
    assert(ll);
    return ll->size() > 0;
};

// TODO
// Returns the data of the next node in the queue
// Equivalent to a peek operation
bool queue::next(unsigned int* next_data) const
{
    assert(ll);

    // Could be [[unlikely]] assuming we're mainly peeking at non-empty queues,
    // really depends on the use case - TODO
    if (!has_next())
    {
        return false;
    }

    *next_data = ll->operator[](0);
    return true;
};

// TODO
// Returns the number of nodes in the queue.
size_t queue::size() const
{
    assert(ll);

    return ll->size();
};

// Registers the malloc function to be used by the queue
void queue::register_malloc(void* (*malloc)(size_t)) { malloc_fptr = malloc; };

// Registers the free function to be used by the queue
void queue::register_free(void (*free)(void*)) { free_fptr = free; };
