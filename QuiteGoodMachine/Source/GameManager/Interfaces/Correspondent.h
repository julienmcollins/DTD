#ifndef CORRESPONDENT_H_
#define CORRESPONDENT_H_

class Correspondence;

/**
 * Correpspondent interface allows anyone who implements this to receive a correspondence 
 */
class Correspondent {
   public:
      /**
       * Receives the correspondence from message buss
       * 
       * @param correspondence The correspondence received
       */
      virtual void ReceiveCorrespondence(const Correspondence& correspondence) = 0;
};

#endif