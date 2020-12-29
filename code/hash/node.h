#ifndef NODE_H
#define NODE_H

template<typename K, typename V>
class Node{
public:
  using allocator_type = std::pmr::polymorphic_allocator<std::byte>;

  Node(allocator_type a = {}):
  myAllocator(a) {}

  Node(const K &key, const V &value,allocator_type a = {}) :
  myAllocator(a),key_(key),value_(value){}

  ~Node(){}

  K getKey() const{
    return key_;
  }

  V getValue() const{
    return value_;
  }

  void setValue(V value){
    value_ = value;
  }

  Node *getNext() const{
    return next_;
  }

  void setNext(Node *next){
    next_ = next;
  }

  //Copy constructor
  Node(const Node& rhs):
    Node(rhs,allocator_type{}){};

  Node(const Node& rhs, allocator_type alloc):
  myAllocator(alloc){
    operator=(rhs);
  }

  //Copy-assign operator
  Node &operator=(const Node& rhs){
      if (value_ != rhs.getValue())
      {
          key_ = rhs.getKey();
          value_ = rhs.getValue();
          next_ = rhs.getNext();
      }
      return *this;
  };

  //Move constructor
  Node(Node&& rhs) :
    Node(std::move(rhs),rhs.get_allocator()) {}

  Node(Node&& rhs, allocator_type alloc) :
    myAllocator(alloc),key_(std::move(rhs.key_)), value_(std::move(rhs.value_)){}

  //Move assign operator
  Node& operator=(Node&& rhs) noexcept{
    if(get_allocator() == rhs.get_allocator()){
      Node(std::move(rhs)).swap(*this);
    }
    else{
      operator=(rhs);
    }
    return *this;
  }

  allocator_type get_allocator() const {return myAllocator;}

private:
  K key_;
  V value_;
  allocator_type myAllocator;
  Node *next_;
};

#endif