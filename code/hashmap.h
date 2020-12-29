#include <functional>
#include<iostream>
#include <memory_resource>
#include <memory>
#include <cstddef>
#include <array>
#include"node.h"
#include"hashfunc.h"
#include"pmrHelper.h"


namespace hmap{
template<typename V,typename K,size_t SIZE = 10, typename FUNC = hashfunc<K,SIZE>>
class hashmap{
public:
  using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

  hashmap(allocator_type a = {}):myAllocator(a),bucket(){}


  V lookup(const K &key){
    auto keyvalue = Hkey(key);
    Node<K,V> *entry = bucket[keyvalue];

    while(entry != NULL){
      if(entry->getKey() == key){
        //value = entry->getValue();
        return entry->getValue();
      }
      entry = entry->getNext();
      }
      return nullptr;
    }

    void bind(const V &value,const K &key){
      auto keyvalue = Hkey(key);
      Node<K,V> *prev = NULL;
      Node<K,V> *entry = bucket[keyvalue];
      while(entry != NULL && entry->getKey() != key){
        prev = entry;
        entry = entry->getNext();
        }
        if(entry == NULL){
          allocator_type alloc = get_allocator();
          auto deleter = [alloc](auto *obj) {
            std::pmr::polymorphic_allocator<std::byte> a{alloc};
            delete_object(a, obj);
          };
          auto obj = new_object<Node<K,V>>(myAllocator,Node<K,V>(key,value,myAllocator));
          entry = std::move(obj);

          //Becomes top of linked list
          if(prev == NULL){ bucket[keyvalue] = entry; }
          //Linkes to previos node
          else{ prev->setNext(entry); }
        }
        //Overwrite if key is occupied
        else{ entry->setValue(value); }
      }

    void remove(const K &key){
      auto keyvalue = Hkey(key);
      Node<K,V> *prev = NULL;
      Node<K,V> *entry = bucket[keyvalue];

      while(entry != NULL && entry->getKey() != key){
        prev = entry;
        entry = entry->getNext();
      }

      if(entry == NULL){
        return;
      }

      else{
        if(prev == NULL){
          bucket[keyvalue] = entry->getNext();
        }
        else{
          prev->setNext(entry->getNext());
        }
        delete_object(myAllocator,entry);
      }
    }

    hashmap(const hashmap& rhs) :
    hashmap(rhs, allocator_type{}){};

    hashmap(const hashmap&rhs, allocator_type alloc):
      myAllocator(alloc){
        operator=(rhs);
      }

    hashmap &operator=(const hashmap& rhs){
      if(!bucket)
      {
        *bucket = new_object<Node<K,V>>(myAllocator,rhs.bucket);
      }
      else{
        //bucket = rhs.bucket;
        *this = rhs;
      }
      return *this;
    }

    hashmap& operator=(hashmap&& rhs)noexcept{
      if(myAllocator == rhs.get_allocator())
      {
        std::swap(this,rhs);
      }
      else{
        operator=(rhs);
      }
    }

    hashmap(hashmap&& rhs) noexcept:
      hashmap(std::move(rhs),rhs.get_allocator()){}

    hashmap(hashmap&& rhs,allocator_type alloc) noexcept:
      myAllocator{alloc}{
        operator= (std::move(rhs));
      }

    allocator_type get_allocator() const {return myAllocator;}

    ~hashmap(){
      //Running monotonic buffer, so all memory is released upon dtor call.
      //calling this->release().
    }

private:
  Node<K,V> *bucket[SIZE];
  std::function<int(K)> Hkey = FUNC();
  allocator_type myAllocator;
  //std::array buffer;
  };
}
