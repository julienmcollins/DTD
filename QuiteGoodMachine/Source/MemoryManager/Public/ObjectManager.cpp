#include "QuiteGoodMachine/Source/MemoryManager/Private/ObjectManager.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>

int ObjectManager::GenerateGlobalID(std::string name) {
   // Check if object already in map and set
   if (global_id_map_.count(name) != 0 &&
       global_id_list_.count(global_id_map_[name]) != 0) {
      return -1;
   }

   // First, create random integer
   int id = rand();

   // Check if inside set
   while (global_id_list_.count(id) != 0) {
      id = rand();
   }

   // Put id into set
   global_id_list_.insert(id);

   // Put id into map
   std::pair<std::string, int> p (name, id);
   global_id_map_.insert(p);

   // Return the guid
   return id;
}

// template <class T>
void *ObjectManager::CreateObject(void *const &object) {
   // Create new object
   // T *new_object = new T();

   // Assign generated global id to it
   // GenerateGlobalID(object);

   // Return the address
   // return new_object;
}

template <class T>
void ObjectManager::DeleteObject(T *object) {
   // Check if not in map and set
   if (global_id_map_.count(object) == 0 &&
       global_id_list_.count(global_id_map_[object]) == 0) {
      return;
   }

   // Remove guid from set
   global_id_list_.erase(global_id_map_[object]);

   // Remove association from map
   global_id_map_.erase(object);

   // Delete
   delete object;
}

int ObjectManager::GetUID(std::string name) {
   if (global_id_map_.count(name) == 0) {
      return -1;
   }

   return global_id_map_[name];
}