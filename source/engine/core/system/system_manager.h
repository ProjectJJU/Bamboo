#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>

namespace Bamboo
{
    /**
     * @brief Base interface for all engine systems
     */
    class IEngineSystem
    {
    public:
        virtual ~IEngineSystem() = default;

        /**
         * @brief Initialize the system
         * @return true if initialization succeeded
         */
        virtual bool initialize() = 0;

        /**
         * @brief Shutdown the system
         */
        virtual void shutdown() = 0;

        /**
         * @brief Get the system name for logging and debugging
         * @return System name
         */
        virtual std::string getName() const = 0;

        /**
         * @brief Get system dependencies (systems that must be initialized before this one)
         * @return Vector of system type names
         */
        virtual std::vector<std::string> getDependencies() const { return {}; }
    };

    /**
     * @brief Manages engine systems and their initialization order
     */
    class SystemManager
    {
    public:
        SystemManager() = default;
        ~SystemManager() = default;

        /**
         * @brief Register a system
         * @tparam TSystem System type
         * @param system System instance
         */
        template<typename TSystem>
        void registerSystem(std::shared_ptr<TSystem> system)
        {
            static_assert(std::is_base_of_v<IEngineSystem, TSystem>, 
                "TSystem must inherit from IEngineSystem");
            
            m_systems.push_back(system);
        }

        /**
         * @brief Initialize all systems in dependency order
         * @return true if all systems initialized successfully
         */
        bool initializeAll();

        /**
         * @brief Shutdown all systems in reverse order
         */
        void shutdownAll();

        /**
         * @brief Get a system by type
         * @tparam TSystem System type
         * @return System instance or nullptr if not found
         */
        template<typename TSystem>
        std::shared_ptr<TSystem> getSystem()
        {
            for (auto& system : m_systems)
            {
                auto typed_system = std::dynamic_pointer_cast<TSystem>(system);
                if (typed_system)
                {
                    return typed_system;
                }
            }
            return nullptr;
        }

    private:
        /**
         * @brief Sort systems by dependencies
         * @return true if sorting succeeded (no circular dependencies)
         */
        bool sortByDependencies();

        std::vector<std::shared_ptr<IEngineSystem>> m_systems;
        std::vector<std::shared_ptr<IEngineSystem>> m_initialized_systems;
    };
}
