#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <type_traits>

namespace Bamboo
{
    /**
     * @brief Service Locator pattern implementation
     * Removes global state dependencies and manages services centrally.
     */
    class ServiceLocator
    {
    public:
        /**
         * @brief Registers a service.
         * @tparam TInterface Service interface type
         * @tparam TImplementation Service implementation type
         * @param implementation Service implementation object
         */
        template<typename TInterface, typename TImplementation>
        void registerService(std::shared_ptr<TImplementation> implementation)
        {
            static_assert(std::is_base_of_v<TInterface, TImplementation>, 
                "TImplementation must inherit from TInterface");
            
            m_services[std::type_index(typeid(TInterface))] = implementation;
        }

        /**
         * @brief Gets a service.
         * @tparam TInterface Service interface type
         * @return shared_ptr to the service object
         */
        template<typename TInterface>
        std::shared_ptr<TInterface> getService()
        {
            auto it = m_services.find(std::type_index(typeid(TInterface)));
            if (it != m_services.end())
            {
                return std::static_pointer_cast<TInterface>(it->second);
            }
            return nullptr;
        }

        /**
         * @brief Checks if a service is registered.
         * @tparam TInterface Service interface type
         * @return true if registered, false otherwise
         */
        template<typename TInterface>
        bool hasService() const
        {
            return m_services.find(std::type_index(typeid(TInterface))) != m_services.end();
        }

        /**
         * @brief Removes a service.
         * @tparam TInterface Service interface type
         */
        template<typename TInterface>
        void unregisterService()
        {
            m_services.erase(std::type_index(typeid(TInterface)));
        }

        /**
         * @brief Removes all services.
         */
        void clear()
        {
            m_services.clear();
        }

        /**
         * @brief Gets the singleton instance.
         * @return ServiceLocator instance
         */
        static ServiceLocator& getInstance()
        {
            static ServiceLocator instance;
            return instance;
        }

    private:
        ServiceLocator() = default;
        ~ServiceLocator() = default;
        ServiceLocator(const ServiceLocator&) = delete;
        ServiceLocator& operator=(const ServiceLocator&) = delete;

        std::unordered_map<std::type_index, std::shared_ptr<void>> m_services;
    };

    /**
     * @brief Service locator shortcut access function
     */
    inline ServiceLocator& Services()
    {
        return ServiceLocator::getInstance();
    }
}
