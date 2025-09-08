#pragma once

#include <chrono>

namespace Bamboo
{
    /**
     * @brief Time management class
     * Separated time-related responsibilities from the Engine class.
     */
    class TimeManager
    {
    public:
        TimeManager();

        /**
         * @brief Calculates and updates delta time.
         * @return Time difference from the previous frame (seconds)
         */
        float tick();

        /**
         * @brief Returns the current FPS.
         * @return FPS value
         */
        int getFPS() const { return m_fps; }

        /**
         * @brief Returns the delta time of the last frame.
         * @return Delta time (seconds)
         */
        float getDeltaTime() const { return m_delta_time; }

        /**
         * @brief Returns the total time since start.
         * @return Total time (seconds)
         */
        float getTotalTime() const { return m_total_time; }

        /**
         * @brief Returns the frame count.
         * @return Total number of frames
         */
        uint64_t getFrameCount() const { return m_frame_count; }

    private:
        void updateFPS(float delta_time);

        std::chrono::steady_clock::time_point m_last_tick_time;
        std::chrono::steady_clock::time_point m_start_time;
        
        float m_delta_time;
        float m_total_time;
        uint64_t m_frame_count;
        
        // FPS calculation variables
        int m_fps;
        float m_fps_timer;
        int m_fps_frame_count;
        
        static constexpr float FPS_UPDATE_INTERVAL = 1.0f; // Update FPS every second
    };
}
