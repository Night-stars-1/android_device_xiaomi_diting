// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdint>

namespace diting::torch {

// Keep this first integration within the 25-100 mA range tested on this phone.
inline constexpr std::array<int, 4> kCurrentsMa = {25, 50, 75, 100};
inline constexpr int32_t kMaxLevel = static_cast<int32_t>(kCurrentsMa.size());
inline constexpr int32_t kDefaultLevel = kMaxLevel;

class Io {
public:
    virtual ~Io() = default;
    virtual int readCurrent(int* value) = 0;
    virtual int readSwitch(int* value) = 0;
    virtual int writeCurrent(int value) = 0;
};

// The caller serializes access. Camera HAL retains ownership of on/off,
// camera arbitration, switch nodes, triggers, and photo/video flash behavior.
class Controller {
public:
    explicit Controller(Io& io) : io_(io) {}

    int setLevel(int32_t level, bool enabled) {
        lastError_ = 0;
        if (!enabled) return 0;  // Never write a nonzero current after HAL off.
        if (level < 1 || level > kMaxLevel) return fail(-EINVAL);
        int powered = 0;
        int status = io_.readSwitch(&powered);
        if (status != 0) return fail(status);
        if (powered <= 0) return fail(-EIO);
        const int requested = kCurrentsMa[static_cast<std::size_t>(level - 1)];
        status = io_.writeCurrent(requested);
        if (status != 0) return fail(status);
        int actual = 0;
        status = io_.readCurrent(&actual);
        if (status != 0) return fail(status);
        if (actual != requested) return fail(-EIO);
        return 0;
    }

    int32_t getLevel() {
        // The framework reads this after the existing void extension setter
        // and propagates errors, instead of claiming a failed write succeeded.
        if (lastError_ != 0) return lastError_;
        int powered = 0;
        int status = io_.readSwitch(&powered);
        if (status != 0) return status;
        if (powered <= 0) return kDefaultLevel;
        int current = 0;
        status = io_.readCurrent(&current);
        if (status != 0) return status;
        if (current <= 0) return -EIO;
        for (std::size_t i = 0; i < kCurrentsMa.size(); ++i) {
            if (current <= kCurrentsMa[i]) return static_cast<int32_t>(i + 1);
        }
        // HAL may initially turn on at its own default (150 mA). The next
        // strength request clamps it to this extension's declared range.
        return kMaxLevel;
    }

private:
    int fail(int status) {
        lastError_ = status < 0 ? status : -EIO;
        return lastError_;
    }
    Io& io_;
    int lastError_ = 0;
};

}  // namespace diting::torch
