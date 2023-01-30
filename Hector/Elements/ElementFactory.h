/*
 *  Hector: a beamline propagation tool
 *  Copyright (C) 2016-2023  Laurent Forthomme
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Hector_Elements_ElementFactory_h
#define Hector_Elements_ElementFactory_h

#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#define BUILDERNM(obj) obj##Builder

/// Add a generic element builder definition
#define REGISTER_ELEMENT(name, obj)                                           \
  namespace hector {                                                          \
    namespace element {                                                       \
      struct BUILDERNM(obj) {                                                 \
        BUILDERNM(obj)() { ElementFactory::get().registerModule<obj>(name); } \
      };                                                                      \
      static const BUILDERNM(obj) gElement##obj;                              \
    }                                                                         \
  }

namespace hector {
  namespace element {
    template <class... Args>
    struct MapHolder {
      static std::map<std::string, std::unique_ptr<Element> (*)(Args...)> CallbackMap;
    };
    template <class... Args>
    std::map<std::string, std::unique_ptr<Element> (*)(Args...)> MapHolder<Args...>::CallbackMap;

    class ElementFactory {
    public:
      static ElementFactory& get() {
        static ElementFactory factory;
        return factory;
      }

      /// Register a named element in the factory
      /// \tparam U Class to register (inherited from element base class)
      template <typename U, typename... Args>
      void registerModule(const std::string& name) {
        static_assert(std::is_base_of<Element, U>::value,
                      "\n\n  *** Failed to register an element with improper inheritance into the factory. ***\n");
        MapHolder<Args...>::CallbackMap[name] = &build<U>;
        /*if (map_.count(name) > 0)
          throw std::invalid_argument("\n\n  *** detected a duplicate element registration with name \"" + name +
                                      "\"! ***\n");
        map_.insert(std::make_pair(name, &build<U>));*/
      }

      /// Constructor type for a module
      //template <typename... Args>
      //using Builder = std::function<std::unique_ptr<Element>(Args...)>;
      //using Builder = std::unique_ptr<Element> (*)(Args...);
      typedef std::unique_ptr<Element> (*Builder)();

      /// Build one instance of a named module
      /// \param[in] name Module name to retrieve
      /// \param[in] params List of parameters to be invoked by the constructor
      template <typename... Args>
      std::unique_ptr<Element> build(const std::string& name, Args... args) const {
        //return map_.at(name)(std::forward<Args>(args)...);
        return MapHolder<Args...>::CallbackMap.at(name)(std::forward<Args>(args)...);
      }

    private:
      ElementFactory() = default;
      /// Register a named element in the factory
      template <typename U, typename... Args>
      static std::unique_ptr<Element> build(Args... args) {
        return std::unique_ptr<Element>(new U(std::forward<Args>(args)...));
      }
      /// Database of modules handled by this instance
      //template <typename... Args>
      //static std::map<std::string, std::unique_ptr<Element> (*)(Args...)> map_;
      //std::unordered_map<std::string, Builder> map_;
    };
  }  // namespace element
}  // namespace hector

#endif
