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
            void reset(double initialG = 0.02, double GRampUpDelay = 7200, double GIncrement = 0.0035);
            
            void setInitialGravity(double value) { initialGravity = value; }
            void setRampUpDelay(double value) { gravityRampUpDelay = value; }
            void setGravityIncrement(double value) { gravityIncrement = value; }

        private:
            std::chrono::steady_clock::time_point start;
            double totalElapsedFrames = 0;
            double gravityAccumulator = 0.0;
            
            double initialGravity;
            double gravityRampUpDelay;
            double gravityIncrement;
            
            // Tick callback (moves piece down by integer rows)
            Callback tickCallback;

            // Helper to compute current gravity
            double _computeCurrentGravity() const;
    };

    class LockDelayTimer {
        public:
            LockDelayTimer();

            void Start();
            void Reset();
            void Cancel();
            bool Update(double deltaTime);
            bool IsActive() const;
            bool IsFirstTouch() const;
            int GetResetsLeft() const;
            void ResetCounter();

        private:
            static constexpr double DELAY_DURATION = 0.5; // 0.5 seconds = 30 logical frames at 60 FPS
            int resetsLeft;
            double elapsed;
            bool active;
            bool firstTouch;
    };
}

#endif UtilFunctions_H