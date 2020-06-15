#ifndef STATEINTERFACE_H_
#define STATEINTERFACE_H_

#include <iostream>

class StateContext;

class StateInterface {
   public:
      /**
       * Constructor takes context (instead of functions)
       */
      StateInterface(StateContext *context) : 
         context_(context) {}

      /**
       * Get the context
       */
      StateContext *GetContext() {
         return context_;
      }

      /**
       * Pretransition --> if you want to transition before doing something
       */
      virtual void PreAction() {}

      /**
       * Performs action --> performs the action
       */
      virtual void PerformAction() = 0;

      /**
       * Posttransition --> if you want to transition after doing something
       */
      virtual void PostAction() {}

      /**
       * Performs action using context
       */
      virtual void DoAction() {
         PreAction();
         PerformAction();
         PostAction();
      }
   
      /**
       * reset function
       */
      virtual void Reset() {}

      /**
       * Transition reset function for during state change
       */
      virtual void PreTransition() {}

      /**
       * Initializer function if just jumped to this state
       */
      virtual void PostTransition() {}

   private:
      // State context pointer
      StateContext *context_;
};

#endif