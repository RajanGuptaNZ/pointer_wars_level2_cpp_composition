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

#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string>

// Some rules for Pointer Wars 2025:
// 0. Implement all functions in linked_list.cc
// 1. Feel free to add members to the classes, but please do not remove
//    any. Doing so will cause test infrastructure to fail to link against
//    your shared library.
// 2. Same goes for the function declarations.
// 3. The malloc_fptr and free_fptr functions are used to allocate
//    memory. Make use of these in your new and delete operators.

// Declaration of the linked_list class.
// Feel free to add additional members as needed, but do not
// remove or delete any.
//
//
class linked_list
{
public:
    // Constructor. Set head to null.
    //
    linked_list();

    // Destructor.
    // Frees all nodes that are present in the linked list.
    //
    virtual ~linked_list();

    // New and delete operators. Needed to support having a
    // custom allocator, which the testing framework uses.
    //
    void* operator new(size_t size);
    void operator delete(void* ptr);

    // Methods to implement.
    // Returns TRUE on succcess, FALSE otherwise.
    //
    bool insert(size_t index, unsigned int data);
    bool insert_front(unsigned int data);
    bool insert_end(unsigned int data);

    // Returns the first index where data is found
    // in the list, SIZE_MAX otherwise.
    //
    size_t find(unsigned int data) const;

    // Returns TRUE if data at index index was able to be
    // removed, FALSE otherwise.
    //
    bool remove(size_t index);

    // Returns the size of the linked list.
    //
    size_t size() const;

    // Inner classes node and iterator.
    //
    struct node
    {
        void* operator new(size_t);
        void operator delete(void*);
        node* next;
        unsigned int data;
    };

    // Operator overloads for access to an individual
    // element in the list. They technically offer more
    // flexibility than level 1 of Pointer Wars in the C
    // language in that you can modify them.
    //
    // Like std::vector, bounds checking isn't performed
    // for operator[], and accessing an element out of
    // bounds is undefined behavior.
    //
    unsigned int& operator[](size_t idx);
    const unsigned int& operator[](size_t idx) const;

    // Static members for memory allocation and free
    // function pointers. Very C like.
    //
    static void register_malloc(void* (*malloc)(size_t));
    static void register_free(void (*free)(void*));
    static void* (*malloc_fptr)(size_t);
    static void (*free_fptr)(void*);

private:
    // Inserts a new node into the head of the list.
    // Assumes list emptiness has been checked.
    bool insert_into_empty_list(unsigned int data);

    // Returns true if the list is empty, false otherwise.
    bool is_empty() const;

    // Creates a new node with the given data and next pointer.
    // Increments the size of the list.
    // Returns nullptr if the node creation fails (size will not be incremented)
    node* create_node(unsigned int data, node* next);

    // Deletes the head node, assumes that list emptiness has been checked
    void remove_head();

    // The head of the linked list.
    //
    node* head{nullptr};

    // If you hate this name, feel free to change it.
    //
    size_t ll_size{0u};
};

#endif
