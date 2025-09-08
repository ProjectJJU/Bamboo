#include "time_manager.h"

namespace Bamboo
{
    TimeManager::TimeManager()
        : m_delta_time(0.0f)
        , m_total_time(0.0f)
        , m_frame_count(0)
        , m_fps(0)
        , m_fps_timer(0.0f)
        , m_fps_frame_count(0)
    {
        auto now = std::chrono::steady_clock::now();
        m_last_tick_time = now;
        m_start_time = now;
    }

    float TimeManager::tick()
    {
        auto current_time = std::chrono::steady_clock::now();
        
        // Calculate delta time
        std::chrono::duration<float> time_span = 
            std::chrono::duration_cast<std::chrono::duration<float>>(current_time - m_last_tick_time);
        m_delta_time = time_span.count();
        
        // Update total time
        std::chrono::duration<float> total_time_span = 
            std::chrono::duration_cast<std::chrono::duration<float>>(current_time - m_start_time);
        m_total_time = total_time_span.count();
        
        // Increment frame count
        m_frame_count++;
        
        // Update FPS
        updateFPS(m_delta_time);
        
        // Store time for next frame
        m_last_tick_time = current_time;
        
        return m_delta_time;
    }

    void TimeManager::updateFPS(float delta_time)
    {
        m_fps_timer += delta_time;
        m_fps_frame_count++;
        
        if (m_fps_timer >= FPS_UPDATE_INTERVAL)
        {
            // FPS = frame count / time
            m_fps = static_cast<int>(m_fps_frame_count / m_fps_timer);
            
            // Reset counters
            m_fps_timer = 0.0f;
            m_fps_frame_count = 0;
        }
    }
}
