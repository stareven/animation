#pragma once

#include "Utils.h"
#include "Interpolator.h"

namespace animation {


class Animation
{
public:
    virtual ~Animation() {}

public: // easy-to-use
    void start() { start(Utils::now()); }
    bool animate() { return animate(Utils::now()); }

    bool started() const { return started(Utils::now()); }
    bool complete() const { return complete(Utils::now()); }
    TTime elapsed() const { return elapsed(Utils::now()); }
    TTime remaining() const { return remaining(Utils::now()); }

public:
    virtual void start(TTime start_time) = 0;
    virtual bool animate(TTime now) = 0;
    virtual void stop() = 0;

public:
    virtual TTime duration() const = 0;
    virtual bool started(TTime now) const = 0;
    virtual bool complete(TTime now) const = 0;
    virtual TTime elapsed(TTime now) const = 0;
    virtual TTime remaining(TTime now) const { return duration() - elapsed(now); }

};


template<typename T>
class PropertyAnimation: public Animation
{
public:
    PropertyAnimation(T &target, T start_state, T complete_state, TTime duration, Interpolator *interpolator)
        : _target(target)
        , _start_state(start_state)
        , _complete_state(complete_state)
        , _duration(duration)
        , _interpolator(interpolator)
        , _start_time(TTimeMax)
    {
        assert(_duration > 0);
        assert(_interpolator);
    }

    PropertyAnimation(T &target, T start_state, T complete_state, TTime duration)
        : PropertyAnimation(target, start_state, complete_state, duration, new LinearInterpolator)
    {}

    PropertyAnimation(T &target, T complete_state, TTime duration, Interpolator *interpolator)
        : PropertyAnimation(target, target, complete_state, duration, interpolator)
    {}

    PropertyAnimation(T &target, T complete_state, TTime duration)
        : PropertyAnimation(target, target, complete_state, duration)
    {}

    virtual ~PropertyAnimation() override
    {
        delete _interpolator;
    }

public: // from Animation
    virtual void start(TTime start_time) override
    {
        _start_time = start_time;
        assert(_start_time < TTimeMax);
    }

    virtual bool animate(TTime now) override
    {
        assert(_start_time < TTimeMax);
        if (!started(now))
        {
            _target = _start_state;
            return true;
        }
        if (complete(now))
        {
            _target = _complete_state;
            return false;
        }
        double ratio = static_cast<double>(elapsed(now)) / duration();
        ratio = _interpolator->value(ratio);
        _target = (1.0 - ratio) * _start_state + ratio * _complete_state;
        return true;
    }

    virtual void stop() override
    {
        _start_time = TTimeMax;
        _target = _complete_state;
    }

public: // from Animation
    virtual TTime duration() const override
    {
        return _duration;
    }

    virtual bool started(TTime now) const override
    {
        return now >= _start_time;
    }

    virtual bool complete(TTime now) const override
    {
        assert(_start_time < TTimeMax);
        return now >= _start_time + duration();
    }

    virtual TTime elapsed(TTime now) const override
    {
        assert(_start_time < TTimeMax);
        if (!started(now)) return 0;
        if (complete(now)) return duration();
        return now - _start_time;
    }

private:
    T &_target;
    const T _start_state;
    const T _complete_state;
    const TTime _duration;
    Interpolator *_interpolator;
    TTime _start_time;

};


}
