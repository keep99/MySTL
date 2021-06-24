#include "profiler.h"

namespace toystl {
    namespace profiler {
        ProfilerInstance::DurationTime ProfilerInstance::duringTime_;
        ProfilerInstance::TimePoint ProfilerInstance::startTime_;
        ProfilerInstance::TimePoint ProfilerInstance::finishTime_;

        void ProfilerInstance::start()
        {
            startTime_ = SteadyClock::now();
        }

        void ProfilerInstance::end()
        {
            finishTime_ = SteadyClock::now();
            duringTime_ = std::chrono::duration_cast<DurationTime>(finishTime_ - startTime_);
        }

        void ProfilerInstance::dumpDuringTime(std::ostream& os)
        {
            os << "total " << duringTime_.count() * 1000 << "ms" << std::endl;
        }

        double ProfilerInstance::second()
        {
            return duringTime_.count();
        }

        double ProfilerInstance::milliSecond()
        {
            return duringTime_.count() * 1000;
        }

        size_t ProfilerInstance::memory(MemoryUnit mu)
        {
            size_t memory = 0;
            struct rusage usage;
            if(getrusage(RUSAGE_SELF, &usage) == -1) {
                throw std::runtime_error("getrusage failed");
            }
            memory = usage.ru_maxrss / 1024;
            switch(mu)
            {
                case MemoryUnit::KB_ :
                    memory = memory KB;
                    break;

                case MemoryUnit::MB_:
                    memory = memory MB;
                    break;

                case MemoryUnit::GB_:
                    memory = memory GB;
                    break;
            }

            return memory;
        }
    }
}