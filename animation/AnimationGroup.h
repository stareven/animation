#include "Animation.h"
#include <list>
#include <algorithm>

namespace animation {


class AnimationGroup: public Animation
{
public:
    virtual void add(Animation *animation) = 0;
    virtual void remove(Animation *animation) = 0;

};


class SequentialAnimationGroup: public AnimationGroup
{
public:
    SequentialAnimationGroup()
        : _duration(0)
        , _start_time(TTimeMax)
    {}

    virtual ~SequentialAnimationGroup() override
    {
        for (Animation *animation: _animations)
        {
            delete animation;
        }
        _animations.clear();
    }

public: // from Animation
    using Animation::start;
    using Animation::animate;
    using Animation::started;
    using Animation::complete;
    using Animation::elapsed;
    using Animation::remaining;

public: // from Animation
    virtual void start(TTime start_time) override
    {
        _start_time = start_time;
        assert(_start_time < TTimeMax);
        TTime offset = _start_time;
        for (Animation *animation: _animations)
        {
            animation->start(offset);
            offset += animation->duration();
        }
    }

    virtual bool animate(TTime now) override
    {
        assert(_start_time < TTimeMax);
        while (!_animations.empty()) {
            Animation *animation = _animations.front();
            if (animation->animate(now)) break;
            delete _animations.front();
            _animations.pop_front();
        }
        return !_animations.empty();
    }

    virtual void stop() override
    {
        _start_time = TTimeMax;
        for (Animation *animation: _animations)
        {
            animation->stop();
            delete animation;
        }
        _animations.clear();
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

public: // from AnimationGroup
    virtual void add(Animation *animation) override
    {
        assert(_start_time == TTimeMax);
        assert(animation);
        auto found = std::find(_animations.begin(), _animations.end(), animation);
        assert(found == _animations.end());
        _animations.push_back(animation);
        _duration += animation->duration();
    }

    virtual void remove(Animation *animation) override
    {
        assert(_start_time == TTimeMax);
        auto found = std::find(_animations.begin(), _animations.end(), animation);
        if (found != _animations.end()) {
            _duration -= animation->duration();
            delete *found;
            _animations.erase(found);
        }
    }

private:
    std::list<Animation*> _animations;
    TTime _duration;
    TTime _start_time;

};


class ParallelAnimationGroup: public AnimationGroup
{
public:
    ParallelAnimationGroup()
        : _duration(0)
        , _start_time(TTimeMax)
    {}

    virtual ~ParallelAnimationGroup() override
    {
        for (Animation *animation: _animations)
        {
            delete animation;
        }
        _animations.clear();
    }

public: // from Animation
    using Animation::start;
    using Animation::animate;
    using Animation::started;
    using Animation::complete;
    using Animation::elapsed;
    using Animation::remaining;

public: // from Animation
    virtual void start(TTime start_time) override
    {
        _start_time = start_time;
        assert(_start_time < TTimeMax);
        for (Animation *animation: _animations)
        {
            animation->start(start_time);
        }
    }

    virtual bool animate(TTime now) override
    {
        assert(_start_time < TTimeMax);
        for (auto it = _animations.begin(); it != _animations.end();) {
            if (!(*it)->animate(now)) {
                delete *it;
                it = _animations.erase(it);
            } else {
                it++;
            }
        }
        return !_animations.empty();
    }

    virtual void stop() override
    {
        _start_time = TTimeMax;
        for (Animation *animation: _animations)
        {
            animation->stop();
            delete animation;
        }
        _animations.clear();
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

public: // from AnimationGroup
    virtual void add(Animation *animation) override
    {
        assert(_start_time == TTimeMax);
        assert(animation);
        auto found = std::find(_animations.begin(), _animations.end(), animation);
        assert(found == _animations.end());
        _animations.push_back(animation);
        _duration = std::max(_duration, animation->duration());
    }

    virtual void remove(Animation *animation) override
    {
        assert(_start_time == TTimeMax);
        auto found = std::find(_animations.begin(), _animations.end(), animation);
        if (found != _animations.end()) {
            delete *found;
            _animations.erase(found);
            _duration = 0;
            for (Animation *animation: _animations)
            {
                _duration = std::max(_duration, animation->duration());
            }
        }
    }

private:
    std::list<Animation*> _animations;
    TTime _duration;
    TTime _start_time;

};


}
