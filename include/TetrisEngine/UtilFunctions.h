#ifndef UtilFunctions_H
#define UtilFunctions_H

#include <chrono>
#include <functional>

namespace tetris {
    class GravityClock {
        public:
            using Callback = std::function<void(int rows)>;
            
            GravityClock(
                double initialG = 0.02, 
                double GRampUpDelay = 7200,
                double GIncrement = 0.0035,
                Callback tickCallback = nullptr
            );

            void update();
            
            void setInitialGravity(double value) { initialGravity = value; }
            void setRampUpDelay(double value) { gravityRampUpDelay = value; }
            void setGravityIncrement(double value) { gravityIncrement = value; }

        private:
            std::chrono::steady_clock::time_point start;
            double totalElapsedFrames = 0;
            double gravityAccumulator = 0.0;
            
            // Configurable parameters
            double initialGravity;
            double gravityRampUpDelay;
            double gravityIncrement;
            
            // Tick callback (moves piece down by integer rows)
            Callback tickCallback;

            // Helper to compute current gravity
            double _computeCurrentGravity() const;
    };
}

#endif UtilFunctions_H