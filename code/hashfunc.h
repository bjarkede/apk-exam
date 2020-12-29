#include <stdio.h>
#include <string.h>

template<typename K,size_t SIZE>
struct hashfunc{
  int operator()(const K& key) const{
    int index = 0;
    int radix = 128;
    int power = 1;
    for(int j = 0; j < strlen(key); j++){
      index = ((index + (key[j])*power) % SIZE) % SIZE;
      power = (power * radix) % SIZE;
    }
    std::cout << "key value: " << index << std::endl;
    return index;
  }
};
