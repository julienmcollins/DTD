#ifndef STATEINTERFACE_H_
#define STATEINTERFACE_H_

class StateContext;

class StateInterface {
   public:
      /**
       * Pretransition --> if you want to transition before doing something
       */
      virtual void PreTransition(StateContext *context) {}

      /**
       * Performs action --> performs the action
       */
      virtual void PerformAction(StateContext *context) = 0;

      /**
       * Posttransition --> if you want to transition after doing something
       */
      virtual void PostTransition(StateContext *context) {}

      /**
       * Performs action using context
       */
      virtual void DoAction(StateContext *context) {
         PreTransition(context);
         PerformAction(context);
         PostTransition(context);
      }
};

#endif