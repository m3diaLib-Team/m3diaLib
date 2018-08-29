#include "core/thread.hpp"

namespace m3d {
    Thread::Thread(void t_function(m3d::ThreadParameter), m3d::ThreadParameter t_parameter, m3d::Thread::Priority t_priority, bool t_autostart, bool t_detached, unsigned long long int t_stackSize) :
                m_stackSize(t_stackSize),
                m_running(false),
                m_started(false),
                m_data({ t_parameter, t_function }) {
        if (t_priority == m3d::Thread::Priority::Default) {
            m_priority = 0x3F - getCurrentPriority() + 1;
        } else {
            int priority = static_cast<int>(t_priority);
            if (priority < 0) priority = 0;
            if (priority > 39) priority = 39;
            m_priority = 0x3F - priority;
        }

        if (t_autostart) {
            start(t_detached);
        }
    }

    Thread::Thread(void t_function(m3d::ThreadParameter), int t_priority, m3d::ThreadParameter t_parameter, bool t_autostart, bool t_detached, unsigned long long int t_stackSize) :
                m_stackSize(t_stackSize),
                m_running(false),
                m_started(false),
                m_data({ t_parameter, t_function }) {
        int priority = static_cast<int>(t_priority);
        if (priority < 0) priority = 0;
        if (priority > 39) priority = 39;
        m_priority = 0x3F - priority;

        if (t_autostart) {
            start(t_detached);
        }
    }

    Thread::~Thread() {
        threadJoin(m_thread, U64_MAX);
        threadFree(m_thread);
    }

    void Thread::setStackSize(unsigned long long int t_stackSize) {
        m_stackSize = t_stackSize;
    }

    void Thread::start(bool t_detached) {
        if (!m_started && !m_running) {
            m_thread = threadCreate(threadFunction, &m_data, m_stackSize, m_priority, -2, t_detached);
            m_started = true;
            m_running = true;
        }
    }

    void Thread::detach() {
        threadDetach(m_thread);
        m_running = false;
        m_started = false;
    }

    void Thread::join(long long unsigned int t_timeout) {
        if (m_started) {
            threadJoin(m_thread, t_timeout);
            threadFree(m_thread);
            m_running = false;
            m_started = false;
        }
    }

   int Thread::getCurrentPriority() {
       s32 prio;
       svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
       return 0x3F - prio;
   }

   // private methods
   void Thread::threadFunction(void* arg) {
       m3d::Thread::ThreadData data = *(m3d::Thread::ThreadData*) arg;
       data.m_function(data.m_parameter);
   }
} /* m3d */