# <img src="https://raw.githubusercontent.com/KitsuneAlex/cxxstreams/master/branding/logo.svg" width="6%"></img> cxxstreams
Are you looking for map, filter, reduce and more on any standard container?  
Do you want it to be a constexpr-ready zero-cost abstraction?  
**Look no further!**

cxxstreams is a highly optimized, constexpr-ready collection stream library for C++20.  
It is a standalone adaption of the **kstd::stream** API found inside the [kstd library](https://git.karmakrafts.dev/kk/kstd)
by Karma Krafts.

### Using cxxstreams
In order to use the stream API, simply make sure you include the cxxstreams header:
```cpp 
#include <cxxstreams/stream.hpp>
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
Now let's take a look at the cxxstreams equivalent to that code:

```cpp 
std::vector<int> elements(...);
const auto sum = cxxstreams::make_stream(elements).sum();
```

As long as the stream value type (`int` in our case) provides an implementation for `operator+` you can use the sum operation
to sum up all the elements operated upon by the stream.

We effectively reduced the amount of code required by three quarters! Of course, you can do a lot more with streams than just 
calculating the sum of a set of elements.