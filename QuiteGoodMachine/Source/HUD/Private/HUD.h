#ifndef HUD_H_
#define HUD_H

#include <vector>
#include <string>
#include <memory>

#include "QuiteGoodMachine/Source/HUD/Interfaces/HUDInterface.h"

class HUD : public HUDInterface {
   public:
      /**
       * Get the list of HUD elements
       * 
       * @return - returns vector of hud elements
       */
      virtual std::vector<std::shared_ptr<HUDElement>> GetHUDElements();

      /**
       * Get a specific element by name
       * 
       * @param string - name of element to get
       * @return Returns shared ptr to the element
       */
      virtual std::shared_ptr<HUDElement> GetHUDElementByName(std::string name);

      /**
       * Add's a HUD element to the HUD
       * Note, this assumes an already created HUD element
       * 
       * @param HUDElement - shared ptr to HUD element
       */
      virtual void AddHUDElement(std::string name, std::shared_ptr<HUDElement> HUD_element);

      /**
       * Creates a HUD element using template and parameter list
       * TODO - create parameter list class
       * 
       * @param ParameterList - parameter list object associated to object
       */
      // template <class T>
      // void CreateHUDElement<T>(ParameterList parameter_list);

      /**
       * Remove a HUD element
       * 
       * @param string - name of the element to remove
       */
      virtual void RemoveHUDElement(std::string name);

      /**
       * Clears the HUD list
       */
      virtual void ClearHUDElementList();

      /**
       * Displays the HUD
       * Displays only if element is active and hud is active
       */
      virtual void DisplayHUD();

      /**
       * Disables the HUD
       */
      virtual void DisableHUD();

      /**
       * Enable the HUD
       */
      virtual void EnableHUD();

      /**
       * Enable HUD element
       * 
       * @param string - name of HUD element to enable
       */
      virtual void EnableHUDElement(std::string name);

      /**
       * Disable HUD element
       * 
       * @param string - name of HUD element to disable
       */
      virtual void DisableHUDElement(std::string name);

      /**
       * Reset hud by resetting all hud elements
       */
      virtual void ResetHUD();

   private:
      // List of hud elements
      std::vector<std::shared_ptr<HUDElement>> HUD_element_list_;

      // map of name to hud element
      std::unordered_map<std::string, std::shared_ptr<HUDElement>> HUD_element_map_;
};

#endif