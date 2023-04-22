# <img src="https://raw.githubusercontent.com/KitsuneAlex/kstd-streams/master/branding/logo.svg" width="6%"></img> kstd-streams

Are you looking for map, filter, reduce and more on any standard container?  
Do you want it to be a constexpr-ready zero-cost abstraction?  
**Look no further!**

kstd-streams is a highly optimized, constexpr-ready collection stream library for C++20.  

### Cloning kstd-streams

If you clone this repository for whatever purpose, don't forget to initialize the submodules using the following command:

```shell
git submodule update --init
```

### Using kstd-streams

In order to use the stream API, simply make sure you include the kstd-streams header:

```cpp
#include <kstd/streams/stream.hpp>
```

### Why streams are amazing

As an example, let's use computing the sum of all elements within a `std::vector<int>`.  
Simple enough, right? If you've got some experience with programming in C++, your solution
would most likely look somewhat like this:

```cpp
std::vector<int> elements(...);
auto sum = 0;
for(const auto element : elements) {
    sum += element;
}
```

That is quite a lot of code for such a simple operation, but that's what you get with a language as verbose as C++..  
Now let's take a look at the kstd-streams equivalent to that code:

```cpp
std::vector<int> elements(...);
const auto sum = kstd::streams::stream(elements).sum();
```

As long as the stream value type (`int` in our case) provides an implementation for `operator+` you can use the sum operation
to sum up all the elements operated upon by the stream.

We effectively reduced the amount of code required by three quarters! Of course, you can do a lot more with streams than just 
calculating the sum of a set of elements.
