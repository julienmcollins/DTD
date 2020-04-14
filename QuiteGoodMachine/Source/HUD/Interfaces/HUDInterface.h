#ifndef HUDINTERFACE_H_
#define HUDINTERFACE_H_

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "QuiteGoodMachine/Source/HUD/Private/HUDElement.h"

class HUDInterface {
   public:
      /**
       * Get the list of HUD elements
       * 
       * @return - returns vector of hud elements
       */
      virtual std::vector<std::shared_ptr<HUDElement>> GetHUDElements() = 0;

      /**
       * Get a specific element by name
       * 
       * @param string - name of element to get
       * @return Returns shared ptr to the element
       */
      virtual std::shared_ptr<HUDElement> GetHUDElementByName(std::string name) = 0;

      /**
       * Add's a HUD element to the HUD
       * Note, this assumes an already created HUD element
       * 
       * @param HUDElement - shared ptr to HUD element
       */
      virtual void AddHUDElement(std::shared_ptr<HUDElement> HUD_element) = 0;

      /**
       * Creates a HUD element using template and parameter list
       * TODO - create parameter list class
       * 
       * @param ParameterList - parameter list object associated to object
       */
      // template <class T>
      // void CreateHUDElement<T>(ParameterList parameter_list) = 0;

      /**
       * Remove a HUD element
       * 
       * @param string - name of the element to remove
       */
      virtual void RemoveHUDElement(std::string name) = 0;

      /**
       * Clears the HUD list
       */
      virtual void ClearHUDElementList() = 0;

      /**
       * Displays the HUD
       * Displays only if element is active and hud is active
       */
      virtual void DisplayHUD() = 0;

      /**
       * Disables the HUD
       */
      virtual void DisableHUD() = 0;

      /**
       * Enable the HUD
       */
      virtual void EnableHUD() = 0;

      /**
       * Enable HUD element
       * 
       * @param string - name of HUD element to enable
       */
      virtual void EnableHUDElement(std::string name) = 0;

      /**
       * Disable HUD element
       * 
       * @param string - name of HUD element to disable
       */
      virtual void DisableHUDElement(std::string name) = 0;
};

#endif