#include "system_manager.h"
#include "engine/core/base/macro.h"
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

namespace Bamboo
{
    bool SystemManager::initializeAll()
    {
        if (m_systems.empty())
        {
            LOG_WARNING("No systems to initialize");
            return true;
        }

        // Sort systems by dependencies
        if (!sortByDependencies())
        {
            LOG_ERROR("Failed to resolve system dependencies");
            return false;
        }

        // Initialize systems in dependency order
        for (auto& system : m_systems)
        {
            LOG_INFO("Initializing system: " + system->getName());
            
            if (!system->initialize())
            {
                LOG_ERROR("Failed to initialize system: " + system->getName());
                
                // Shutdown already initialized systems
                shutdownAll();
                return false;
            }
            
            m_initialized_systems.push_back(system);
            LOG_INFO("Successfully initialized system: " + system->getName());
        }

        LOG_INFO("All systems initialized successfully");
        return true;
    }

    void SystemManager::shutdownAll()
    {
        // Shutdown in reverse order
        for (auto it = m_initialized_systems.rbegin(); it != m_initialized_systems.rend(); ++it)
        {
            LOG_INFO("Shutting down system: " + (*it)->getName());
            
            try
            {
                (*it)->shutdown();
                LOG_INFO("Successfully shut down system: " + (*it)->getName());
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("Exception during shutdown of system " + (*it)->getName() + ": " + e.what());
            }
        }

        m_initialized_systems.clear();
        LOG_INFO("All systems shut down");
    }

    bool SystemManager::sortByDependencies()
    {
        if (m_systems.size() <= 1)
        {
            return true; // No sorting needed
        }

        // Create a map of system names to systems
        std::unordered_map<std::string, std::shared_ptr<IEngineSystem>> system_map;
        for (auto& system : m_systems)
        {
            system_map[system->getName()] = system;
        }

        // Topological sort using Kahn's algorithm
        std::unordered_map<std::string, int> in_degree;
        std::unordered_map<std::string, std::vector<std::string>> adj_list;

        // Initialize in-degree and adjacency list
        for (auto& system : m_systems)
        {
            std::string name = system->getName();
            in_degree[name] = 0;
            adj_list[name] = {};
        }

        // Build dependency graph
        for (auto& system : m_systems)
        {
            std::string name = system->getName();
            auto dependencies = system->getDependencies();
            
            for (const auto& dep : dependencies)
            {
                if (system_map.find(dep) == system_map.end())
                {
                    LOG_ERROR("System " + name + " depends on unknown system: " + dep);
                    return false;
                }
                
                adj_list[dep].push_back(name);
                in_degree[name]++;
            }
        }

        // Find all systems with no dependencies
        std::vector<std::string> queue;
        for (auto& [name, degree] : in_degree)
        {
            if (degree == 0)
            {
                queue.push_back(name);
            }
        }

        // Process systems in topological order
        std::vector<std::shared_ptr<IEngineSystem>> sorted_systems;
        
        while (!queue.empty())
        {
            std::string current = queue.back();
            queue.pop_back();
            
            sorted_systems.push_back(system_map[current]);
            
            // Reduce in-degree for dependent systems
            for (const auto& dependent : adj_list[current])
            {
                in_degree[dependent]--;
                if (in_degree[dependent] == 0)
                {
                    queue.push_back(dependent);
                }
            }
        }

        // Check for circular dependencies
        if (sorted_systems.size() != m_systems.size())
        {
            LOG_ERROR("Circular dependency detected in systems");
            return false;
        }

        m_systems = std::move(sorted_systems);
        return true;
    }
}
