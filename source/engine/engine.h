#pragma once

#include <memory>

namespace Bamboo
{
    // Forward declarations
    class TimeManager;
    /**
     * @brief Main engine class responsible for the game loop and system coordination
     * Refactored to follow single responsibility principle
     */
    class Engine
    {
    public:
        Engine();
        ~Engine();

        /**
         * @brief Initializes the engine and all subsystems
         * @return true if initialization succeeded, false otherwise
         */
        bool initialize();

        /**
         * @brief Shuts down the engine and cleans up resources
         */
        void shutdown();

        /**
         * @brief Runs one frame of the engine
         * @return true if the engine should continue running, false to exit
         */
        bool tick();

        /**
         * @brief Checks if the engine is running
         * @return true if running, false otherwise
         */
        bool isRunning() const { return m_is_running; }

        /**
         * @brief Gets the current FPS
         * @return Current frames per second
         */
        int getFPS() const;

        /**
         * @brief Gets the delta time of the last frame
         * @return Delta time in seconds
         */
        float getDeltaTime() const;

    private:
        /**
         * @brief Initializes the log system first (before any LOG macros can be used)
         * @return true if log system initialized successfully
         */
        bool initializeLogSystem();

        /**
         * @brief Initializes all engine subsystems in proper order
         * @return true if all systems initialized successfully
         */
        bool initializeSystems();

        /**
         * @brief Shuts down all systems in reverse order
         */
        void shutdownSystems();

        /**
         * @brief Updates logic systems
         * @param delta_time Time since last frame
         */
        void updateLogic(float delta_time);

        /**
         * @brief Updates rendering system
         * @param delta_time Time since last frame
         */
        void updateRender(float delta_time);

        bool m_is_running;
        bool m_is_initialized;
        
        std::shared_ptr<class TimeManager> m_time_manager;
    };
}