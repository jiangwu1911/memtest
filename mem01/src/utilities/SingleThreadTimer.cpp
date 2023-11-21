// ================================================================================================
//
// If not explicitly stated: Copyright (C) 2016, all rights reserved,
//      Rüdiger Göbl
//		Email r.goebl@tum.de
//      Chair for Computer Aided Medical Procedures
//      Technische Universität München
//      Boltzmannstr. 3, 85748 Garching b. München, Germany
//
// ================================================================================================

#include "SingleThreadTimer.h"
#include <cmath>
#include <thread>

using namespace std;
using namespace std::chrono;

BEGIN_NAMESPACE_ESI

SingleThreadTimer::SingleThreadTimer()
    : m_frequency(0), m_slotDuration(0), m_initialized(false), m_firstRunDone(false) {}

double SingleThreadTimer::getFrequency() { return m_frequency; }

void SingleThreadTimer::setFrequency(double frequency) {
    m_frequency = frequency;
    m_beginLastSlot = clock::now();
    m_slotDuration = duration_cast<duration>(microseconds((long long)round(1e6 / frequency)));
    m_initialized = true;
    m_firstRunDone = false;
}

void SingleThreadTimer::sleepUntilNextSlot() {
    if (m_initialized) {
        if (m_firstRunDone) {
            // 有一种情况，例如系统时间向前调了1小时，会造成sleep_until()睡眠1小时后，才会返回, 避免这么用。
            // clock::time_point beginNextSlot = m_beginLastSlot + m_slotDuration;
            // this_thread::sleep_until(beginNextSlot);
            this_thread::sleep_for(m_slotDuration);

        } else {
            m_firstRunDone = true;
        }
        m_beginLastSlot = clock::now();
    }
}

END_NAMESPACE_ESI
