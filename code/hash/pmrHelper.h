#ifndef PMRHELPER_H
#define PMRHELPER_H

//helper functions from lecture and cppref - only implemented in c20
template<typename ALLOC, typename T>
void delete_object(ALLOC alloc, T* t){
  alloc.destroy(t);
  alloc.resource()->deallocate(t,
                                sizeof(T),
                                alignof(T));
}

template<typename T, typename ALLOC, typename... CtorArgs>
T* new_object(ALLOC alloc, CtorArgs&&... ctor_args){
  T* t = static_cast<T*>(alloc.resource()->allocate(sizeof(T),
                                                    alignof(T)));
  try{
    alloc.construct(t,std::forward<CtorArgs>(ctor_args)...);
  }
  catch(...){
    alloc.resource()->deallocate(t, sizeof(T),alignof(T));
  }
  return t;
}

#endif