#include "engine.h"
#include "engine/core/base/macro.h"
#include "engine/core/time/time_manager.h"
#include "engine/core/service/service_locator.h"
#include "engine/function/global/engine_context.h"
#include "engine/function/render/window_system.h"
#include "engine/function/render/render_system.h"
#include "engine/function/framework/world/world_manager.h"
#include "engine/core/event/event_system.h"
#include "engine/platform/timer/timer.h"
#include "engine/core/log/log_system.h"
#include "engine/platform/file/file_system.h"
#include <iostream>

namespace Bamboo
{
    Engine::Engine()
        : m_is_running(false)
        , m_is_initialized(false)
        , m_time_manager(nullptr)
    {
    }

    Engine::~Engine()
    {
        if (m_is_initialized)
        {
            shutdown();
        }
    }

    bool Engine::initialize()
    {
        if (m_is_initialized)
        {
            // Use standard output before log system is initialized
            std::cout << "[WARNING] Engine already initialized" << std::endl;
            return true;
        }

        // Initialize basic log system first (before any LOG_ macros)
        if (!initializeLogSystem())
        {
            std::cerr << "[ERROR] Failed to initialize log system" << std::endl;
            return false;
        }

        LOG_INFO("Initializing engine...");

        // Create time manager
        m_time_manager = std::make_shared<TimeManager>();
        if (!m_time_manager)
        {
            LOG_ERROR("Failed to create TimeManager");
            return false;
        }

        // Register time manager as a service
        Services().registerService<TimeManager>(m_time_manager);

        // Initialize all other subsystems
        if (!initializeSystems())
        {
            LOG_ERROR("Failed to initialize engine systems");
            shutdown();
            return false;
        }

        m_is_initialized = true;
        m_is_running = true;

        LOG_INFO("Engine initialized successfully");
        return true;
    }

    void Engine::shutdown()
    {
        if (!m_is_initialized)
        {
            return;
        }

        LOG_INFO("Shutting down engine...");

        m_is_running = false;

        // Shutdown all systems
        shutdownSystems();

        // Clear all services
        Services().clear();

        m_time_manager.reset();
        m_is_initialized = false;

        LOG_INFO("Engine shutdown complete");
    }

    bool Engine::tick()
    {
        if (!m_is_running || !m_is_initialized)
        {
            return false;
        }

        // Update time and get delta time
        float delta_time = m_time_manager->tick();

        // Update engine context delta time for legacy compatibility
        g_engine.setDeltaTime(delta_time);

        // Update systems
        updateLogic(delta_time);
        updateRender(delta_time);

        // Update window title with FPS
        if (g_engine.windowSystem())
        {
            g_engine.windowSystem()->pollEvents();
            g_engine.windowSystem()->setTitle(
                std::string(APP_NAME) + " - " + std::to_string(getFPS()) + " FPS");
            
            // Check if window should close
            if (g_engine.windowSystem()->shouldClose())
            {
                m_is_running = false;
            }
        }

        return m_is_running;
    }

    int Engine::getFPS() const
    {
        return m_time_manager ? m_time_manager->getFPS() : 0;
    }

    float Engine::getDeltaTime() const
    {
        return m_time_manager ? m_time_manager->getDeltaTime() : 0.0f;
    }

    bool Engine::initializeLogSystem()
    {
        try
        {
            // Initialize file system first (needed for log system)
            auto file_system = std::make_shared<FileSystem>();
            if (!file_system)
            {
                return false;
            }
            file_system->init();
            g_engine.setFileSystem(file_system);

            // Initialize log system
            auto log_system = std::make_shared<LogSystem>();
            if (!log_system)
            {
                return false;
            }
            log_system->init();
            g_engine.setLogSystem(log_system);

            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }

    bool Engine::initializeSystems()
    {
        // Initialize engine context (contains all remaining subsystems)
        try
        {
            g_engine.init();
            return true;
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Exception during systems initialization: " + std::string(e.what()));
            return false;
        }
    }

    void Engine::shutdownSystems()
    {
        try
        {
            g_engine.destroy();
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("Exception during systems shutdown: " + std::string(e.what()));
        }
    }

    void Engine::updateLogic(float delta_time)
    {
        if (g_engine.eventSystem())
        {
            g_engine.eventSystem()->tick();
        }

        if (g_engine.worldManager())
        {
            g_engine.worldManager()->tick(delta_time);
        }

        if (g_engine.timerManager())
        {
            g_engine.timerManager()->tick(delta_time);
        }
    }

    void Engine::updateRender(float delta_time)
    {
        if (g_engine.renderSystem())
        {
            g_engine.renderSystem()->tick(delta_time);
        }
    }
}