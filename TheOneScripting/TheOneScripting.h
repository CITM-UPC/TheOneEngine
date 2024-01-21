#pragma once

#ifdef THEONESCRIPTING_EXPORTS
#define THEONESCRIPTING_API __declspec(dllexport)
#else
#define THEONESCRIPTING_API __declspec(dllimport)
#endif

// Initialize a Fibonacci relation sequence
// such that F(0) = a, F(1) = b.
// This function must be called before any other function.
extern "C" THEONESCRIPTING_API void _init(
    const unsigned long long a, const unsigned long long b);

// Produce the next value in the sequence.
// Returns true on success and updates current value and index;
// false on overflow, leaves current value and index unchanged.
extern "C" THEONESCRIPTING_API bool fibonacci_next();

// Get the current value in the sequence.
extern "C" THEONESCRIPTING_API unsigned long long fibonacci_current();

// Get the position of the current value in the sequence.
extern "C" THEONESCRIPTING_API unsigned fibonacci_index();