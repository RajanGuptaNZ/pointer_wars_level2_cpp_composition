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

#include "linked_list.h"

#include <cassert>

// Initial declaration of the static member function
// pointers in the linked_list class.
//
void* (*linked_list::malloc_fptr)(size_t) = nullptr;
void (*linked_list::free_fptr)(void*) = nullptr;

// Constructor. Relies on default initialization of head and ll_size.
linked_list::linked_list() {}

// Iterate through the list and delete each node.
linked_list::~linked_list()
{
    node* currentNode = head;
    for (size_t i = 0u; i < ll_size; ++i)
    {
        node* nextNode = currentNode->next;
        delete (currentNode);
        currentNode = nextNode;
    }
}

// Use the malloc_fptr to allocate memory for the new node.
void* linked_list::operator new(size_t size)
{
    assert(linked_list::free_fptr);
    return malloc_fptr(size);
}

// Use the free_fptr to free the memory of a node
void linked_list::operator delete(void* ptr)
{
    assert(linked_list::free_fptr);
    free_fptr(ptr);
}

// Create a new node with the given data and set the head to the new node.
// Should only be called if the list is empty.
bool linked_list::insert_into_empty_list(unsigned int data)
{
    head = create_node(data, nullptr);
    if (head == nullptr) [[unlikely]]
    {
        return false;
    }

    return true;
}

// Create a new node with the given data and set the next node to the given
// node.
linked_list::node* linked_list::create_node(unsigned int data, node* next)
{
    node* newNode = new node();
    if (newNode == nullptr) [[unlikely]]
    {
        return nullptr;
    }

    newNode->data = data;
    newNode->next = next;
    ++ll_size;
    return newNode;
}

// Insert a new node at the given index.
// If the index is greater than the size of the list, return false.
// If the list is empty, insert the new node at the head.
// If the index is equal to the size of the list, insert the new node at the
// end. If the index is 0, insert the new node at the head. Otherwise, insert
// the new node at the given index.
bool linked_list::insert(size_t index, unsigned int data)
{
    if (index > ll_size) [[unlikely]]
    {
        return false;
    }

    if (is_empty())
    {
        return insert_into_empty_list(data);
    }

    if (index == ll_size)
    {
        return insert_end(data);
    }

    if (index == 0)
    {
        return insert_front(data);
    }

    // Find the node immediately before the desired index
    node* prevNode = head;
    for (size_t i = 1u; i < index; ++i)
    {
        prevNode = prevNode->next;
        assert(prevNode != nullptr);
    }

    node* newNode = create_node(data, prevNode->next);
    prevNode->next = newNode;

    return true;
}

// Insert a new node at the front of the list.
// Can handle an empty list.
bool linked_list::insert_front(unsigned int data)
{
    node* newNode = create_node(data, head);
    if (newNode == nullptr) [[unlikely]]
    {
        return false;
    }

    head = newNode;

    return true;
}

// Insert a new node at the end of the list.
// Can NOT handle an empty list.
bool linked_list::insert_end(unsigned int data)
{
    if (is_empty())
    {
        return insert_into_empty_list(data);
    }

    node* newNode = create_node(data, nullptr);
    if (newNode == nullptr) [[unlikely]]
    {
        return false;
    }

    node* currentNode = head;
    for (size_t i = 2; i < ll_size; ++i)
    {
        currentNode = currentNode->next;
    }

    currentNode->next = newNode;

    return true;
}

// Iterate through the list and return the index of the first node with the
// given data.
// If the data is not found, return SIZE_MAX.
size_t linked_list::find(unsigned int data) const
{
    node* currentNode = head;
    for (size_t i = 0u; i < ll_size; ++i)
    {
        if (currentNode->data == data)
            return i;

        currentNode = currentNode->next;
    }

    return SIZE_MAX;
}

// Remove the node at the given index.
// If the index is greater than the size of the list, return false.
// If the index is 0, remove the head.
// Otherwise, remove the node at the given index.
bool linked_list::remove(size_t index)
{
    // Find the node immediately before the desired index.
    // Then orphan the desired node by linking prevNode -> nextNode
    // And then cleanup the orphaned node.

    if (is_empty() || index >= ll_size) [[unlikely]]
    {
        return false;
    }

    if (index == 0)
    {
        remove_head();
        return true;
    }

    node* prevNode = head;
    for (size_t i = 1u; i < index; ++i)
    {
        prevNode = prevNode->next;
    }

    node* currentNode = prevNode->next;

    // orphan the currentNode
    prevNode->next = currentNode->next;

    // cleanup
    --ll_size;
    delete (currentNode);

    return true;
}

// Remove the head of the list.
// Can NOT handle an empty list.
void linked_list::remove_head()
{
    node* newHead = head->next;
    delete (head);
    head = newHead;
    --ll_size;
}

size_t linked_list::size() const { return ll_size; }

// Iterate through the list and return a reference to the data of the node at
// the given index.
unsigned int& linked_list::operator[](size_t idx)
{
    node* currentNode = head;
    for (size_t i = 0; i < idx; ++i)
    {
        currentNode = currentNode->next;
    }

    return currentNode->data;
}

// Iterate through the list and return a const reference to the data of the
// node at the given index.
const unsigned int& linked_list::operator[](size_t idx) const
{
    node* currentNode = head;
    for (size_t i = 0; i < idx; ++i)
    {
        currentNode = currentNode->next;
    }

    return currentNode->data;
}

// Register the malloc_fptr
void linked_list::register_malloc(void* (*malloc)(size_t))
{
    linked_list::malloc_fptr = malloc;
}

// Register the free_fptr
void linked_list::register_free(void (*free)(void*))
{
    linked_list::free_fptr = free;
}

// Use the malloc_fptr to allocate memory for the new node.
void* linked_list::node::operator new(size_t size)
{
    assert(linked_list::malloc_fptr);
    void* ptr = linked_list::malloc_fptr(size);

    return ptr;
}

// Use the free_fptr to free the memory of a node
void linked_list::node::operator delete(void* ptr)
{
    assert(linked_list::free_fptr);
    return linked_list::free_fptr(ptr);
}

// Check if the list is empty.
// IE is the head nullptr
bool linked_list::is_empty() const
{
    assert((head == nullptr) == (ll_size == 0u));
    return head == nullptr;
}
