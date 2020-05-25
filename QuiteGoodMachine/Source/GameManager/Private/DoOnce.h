#ifndef DOONCE_H_
#define DOONCE_H_

#include <functional>

class DoOnce {
   public:
      /**
       * DoOnce functions calls passed in function
       */
      DoOnce operator()(std::function<void()> func) {
         if (!done_) {
            func();
            done_ = true;
         }
      }

      /**
       * Reset function for allowing do once to repeate
       */
      void Reset() {
         done_ = false;
      }

      /**
       * Check if locked
       */
      bool Locked() {
         return (done_ == true);
      }

   private:
      // Flag for doing once
      bool done_ = false;
};

#endif