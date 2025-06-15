#include "TetrisEngine/UtilFunctions.h"

static constexpr double MicrosecondsToSeconds = 0.000001;
static constexpr double FramesPerSecond = 60.0;

namespace tetris {
    GravityClock::GravityClock(
        double initialG,
        double GRampUpDelay,
        double GIncrement,
        Callback callback
    ) : start(std::chrono::steady_clock::now()),
        initialGravity(initialG),
        gravityRampUpDelay(GRampUpDelay),
        gravityIncrement(GIncrement),
        tickCallback(std::move(callback))
    {}

    void GravityClock::update() {
        auto now = std::chrono::steady_clock::now();
        std::chrono::microseconds elapsedMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
        double elapsedFrames = static_cast<double>(elapsedMicroseconds.count()) * MicrosecondsToSeconds * FramesPerSecond;
        
        start = now;
        totalElapsedFrames += elapsedFrames;

        double gravityPerFrame = _computeCurrentGravity();
        
        gravityAccumulator += gravityPerFrame * elapsedFrames;
        
        if (gravityAccumulator >= 1.0) {
            int rowsToMove = static_cast<int>(gravityAccumulator);
            gravityAccumulator -= rowsToMove;
            
            if (tickCallback) {
                tickCallback(rowsToMove);
            }
        }
    }

    double GravityClock::_computeCurrentGravity() const {
        if (totalElapsedFrames < gravityRampUpDelay) {
            return initialGravity;
        }
        double secondsAfterDelay = (totalElapsedFrames - gravityRampUpDelay) / FramesPerSecond;
        int increments = static_cast<int>(secondsAfterDelay);
        return initialGravity + increments * gravityIncrement;
    }

    void GravityClock::reset(double initialG, double GRampUpDelay, double GIncrement) {
        totalElapsedFrames = 0;
        gravityAccumulator = 0.0;

        initialGravity = initialG;
        gravityRampUpDelay = GRampUpDelay;
        gravityIncrement = GIncrement;
    }
}