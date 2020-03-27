#ifndef OBJECTMANAGER_H_
#define OBJECTMANAGER_H_

#include <unordered_map>
#include <unordered_set>
#include <string>

class ObjectManager {
   public:
      /**
       * Grabs the singleton instance
       */
      static ObjectManager& GetInstance() {
         static ObjectManager instance;
         return instance;
      }

      /**
       * Allocate object
       */
      // template <class T>
      void *CreateObject(void *const &object);

      /**
       * Deallocate object
       */
      template <class T>
      void DeleteObject(T *object);

      /**
       * Generates a unique global id
       */
      int GenerateGlobalID(void *const &object);

   private:
      /**
       * Private constructor
       */ 
      ObjectManager() {};

      /**
       * Object ID Set that holds all of the known global ids
       */
      std::unordered_set<int> global_id_list_;

      /**
       * Object ID Map that holds all the pairings between objects and guids
       */
      std::unordered_map<void *, int> global_id_map_;

   public:
      ObjectManager(ObjectManager const&) = delete;
      void operator=(ObjectManager const&) = delete;
};

#endif