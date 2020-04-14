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
      int GenerateGlobalID(std::string name);

      /**
       * Gets uid from name
       * 
       * @param name String with name of desired object
       * @return int Returns the id associated to the name (if it exists)
       */
      int GetUID(std::string name);

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
      std::unordered_map<std::string, int> global_id_map_;

   public:
      ObjectManager(ObjectManager const&) = delete;
      void operator=(ObjectManager const&) = delete;
};

#endif