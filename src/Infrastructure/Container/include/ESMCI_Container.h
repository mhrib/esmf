// $Id: ESMCI_Container.h,v 1.5 2011/05/06 18:01:44 theurich Exp $
//
// Earth System Modeling Framework
// Copyright 2002-2011, University Corporation for Atmospheric Research,
// Massachusetts Institute of Technology, Geophysical Fluid Dynamics
// Laboratory, University of Michigan, National Centers for Environmental
// Prediction, Los Alamos National Laboratory, Argonne National Laboratory,
// NASA Goddard Space Flight Center.
// Licensed under the University of Illinois-NCSA License.
//
//==============================================================================

// these lines prevent this file from being read more than once if it
// ends up being included multiple times

#ifndef ESMCI_Container_H
#define ESMCI_Container_H

#include <map>
#include <vector>
#include <iostream>

// LogErr headers
#include "ESMCI_LogErr.h"                  // for LogErr
#include "ESMF_LogMacros.inc"             // for LogErr


namespace ESMCI {

  template <typename Key, typename T>
  class Container : public std::map<Key, T> {
   public:
    bool add(Key k, T t, bool relaxed=false);
    void addReplace(Key k, T t);
    void remove(Key k, bool relaxed=false);
    void replace(Key k, T t, bool relaxed=false);
    T get(Key k);
    void getVector(std::vector<T> &v)const;
    void getKeyVector(std::vector<Key> &v)const;
    bool isPresent(Key k)const{
      if (this->find(k)!=this->end())
        return true;  // key found
      return false;   // key not found
    }
    void print()const;
  };
  
#undef  ESMC_METHOD
#define ESMC_METHOD "ESMCI::Container::add()"
  // Add an element to the container. By default the method executes in strict
  // mode, where it is an error if an element with the same key already exists.
  // In relaxed mode this condition turns this method into a no-op and no
  // error is thrown.
  template <typename Key, typename T>
  bool Container<Key, T>::add(Key k, T t, bool relaxed){
    int rc = ESMC_RC_NOT_IMPL;              // final return code
    bool added;
    if (this->find(k)!=this->end()){
      // already exists
      added = false;  // indicate not added
      if (!relaxed){
        ESMC_LogDefault.MsgFoundError(ESMC_RC_ARG_BAD,
          "key already exists", &rc);
        throw rc;  // bail out with exception
      }
    }else{
      added = true;  // indicate added
      (*this)[k]=t;
    }
    return added;
  }
  
#undef  ESMC_METHOD
#define ESMC_METHOD "ESMCI::Container::addReplace()"
  // Add an element to the container if no element with the specified key
  // exists, or replace an existing element with the same key.
  template <typename Key, typename T>
  void Container<Key, T>::addReplace(Key k, T t){
    (*this)[k]=t;
  }

#undef  ESMC_METHOD
#define ESMC_METHOD "ESMCI::Container::remove()"
  // Remove the element with specified key. By default the method executes in
  // strict mode, where it is an error if no element with the specified key
  // exists. In relaxed mode this condition turns this method into a no-op
  // and no error is thrown.
  template <typename Key, typename T>
  void Container<Key, T>::remove(Key k, bool relaxed){
    int rc = ESMC_RC_NOT_IMPL;              // final return code
    typename Container::iterator pos = this->find(k);
    if (pos==this->end()){
      // does not exist
      if (!relaxed){
        ESMC_LogDefault.MsgFoundError(ESMC_RC_ARG_BAD,
          "key does not exist", &rc);
        throw rc;  // bail out with exception
      }
    }else
      this->erase(pos);
  }
  
#undef  ESMC_METHOD
#define ESMC_METHOD "ESMCI::Container::replace()"
  // Replace the element that has matching key with the specified element. By
  // default the method executes in strict mode, where it is an error if no
  // element with the same key as the specified element exists. In relaxed mode
  // this condition turns this method into a no-op and no error is thrown.
  template <typename Key, typename T>
  void Container<Key, T>::replace(Key k, T t, bool relaxed){
    int rc = ESMC_RC_NOT_IMPL;              // final return code
    typename Container::iterator pos = this->find(k);
    if (pos==this->end()){
      // does not exist
      if (!relaxed){
        ESMC_LogDefault.MsgFoundError(ESMC_RC_ARG_BAD,
          "key does not exist", &rc);
        throw rc;  // bail out with exception
      }
    }else
      (*this)[k]=t;
  }

#undef  ESMC_METHOD
#define ESMC_METHOD "ESMCI::Container::get()"
  // Access an element by key. It is an error if no element with the specified
  // key exists.
  template <typename Key, typename T>
  T Container<Key, T>::get(Key k){
    int rc = ESMC_RC_NOT_IMPL;              // final return code
    typename Container::const_iterator pos = this->find(k);
    if (pos==this->end()){
      // does not exist -> error
      ESMC_LogDefault.MsgFoundError(ESMC_RC_ARG_BAD,
        "key does not exist", &rc);
      throw rc;  // bail out with exception
    }
    return (*this)[k];
  }

#undef  ESMC_METHOD
#define ESMC_METHOD "ESMCI::Container::getVector()"
  // Access the entire contents of the container in form of a vector.
  template <typename Key, typename T>
  void Container<Key, T>::getVector(std::vector<T> &v)const{
    int rc = ESMC_RC_NOT_IMPL;              // final return code
    v.resize(this->size());
    typename Container::const_iterator pos;
    int i = 0;
    for (pos = this->begin(); pos != this->end(); ++pos)
      v[i++] = pos->second;
  }

#undef  ESMC_METHOD
#define ESMC_METHOD "ESMCI::Container::getKeyVector()"
  // Access the keys of the entire contents of the container in form of a
  // vector.
  template <typename Key, typename T>
  void Container<Key, T>::getKeyVector(std::vector<Key> &v)const{
    int rc = ESMC_RC_NOT_IMPL;              // final return code
    v.resize(this->size());
    typename Container::const_iterator pos;
    int i = 0;
    for (pos = this->begin(); pos != this->end(); ++pos)
      v[i++] = pos->first;
  }

#undef  ESMC_METHOD
#define ESMC_METHOD "ESMCI::Container::print()"
  // Print the contents of the container.
  template <typename Key, typename T>
  void Container<Key, T>::print()const{
    int rc = ESMC_RC_NOT_IMPL;              // final return code
    typename Container::const_iterator pos;
    int i = 0;
    for (pos = this->begin(); pos != this->end(); ++pos)
      std::cout << "Container::print() item="<<i++<<" key="<<pos->first
        <<" value="<<pos->second<<"\n";
  }

} // namespace ESMCI

#endif  // ESMCI_Container_H
