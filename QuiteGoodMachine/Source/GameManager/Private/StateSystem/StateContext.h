#ifndef STATECONTEXT_H_
#define STATECONTEXT_H_

#include <memory>
#include <unordered_map>

class PositionalElement;
class StateInterface;
class Animation;

class StateContext {
   public:
      /**
       * Constructor
       */
      StateContext(PositionalElement *base);

      /**
       * Get the base pointer
       */
      PositionalElement *GetBase() const;

      /**
       * Set the state
       */
      void SetState(std::shared_ptr<StateInterface> state);

      /**
       * Get the state
       */
      std::shared_ptr<StateInterface> GetCurrentState();

      /**
       * Get the state by name
       */
      std::shared_ptr<StateInterface> GetState(std::string name);

      /**
       * Register a state to the context
       */
      void RegisterState(std::string name, std::shared_ptr<StateInterface> new_state);

      /**
       * Does the action associated to the current state
       */
      void DoAction();

   private:
      // Pointer to calling object --> used for gathering info
      PositionalElement *base_;

      // Current associated state
      std::shared_ptr<StateInterface> current_state_;

      // Set of states owned by the context
      std::unordered_map<std::string, std::shared_ptr<StateInterface>> registered_states_;

};

#endif