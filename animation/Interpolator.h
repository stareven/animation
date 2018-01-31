#pragma once

#include "Utils.h"

namespace animation {


class Interpolator
{
public:
    virtual ~Interpolator() {}

public:
    double value(double ratio) const
    {
        double normalized = _normalize(ratio);
        return _value(normalized);
    }

private:
    double _normalize(double ratio) const
    {
        if (ratio < 0.0) return 0.0;
        if (ratio > 1.0) return 1.0;
        return ratio;
    }

    virtual double _value(double ratio) const = 0;

};


class LinearInterpolator: public Interpolator
{
private:
    virtual double _value(double ratio) const override
    {
        return ratio;
    }

};


class PowerInterpolator: public Interpolator
{
public:
    PowerInterpolator(double power=2): _power(power) {}

private:
    virtual double _value(double ratio) const override
    {
        return pow(ratio, _power);
    }

private:
    double _power;

};

class BackInterpolator: public Interpolator
{
public:
    BackInterpolator(double overflow=1.70158, double power=2)
        : _overflow(overflow), _power(power) {}

private:
    virtual double _value(double ratio) const override
    {
        return ((_overflow + 1) * pow(ratio, _power + 1) -
                _overflow * pow(ratio, _power));
    }

private:
    double _overflow;
    double _power;

};


class SineInterpolator: public Interpolator
{
private:
    virtual double _value(double ratio) const override
    {
        return sin(ratio * M_PI_2);
    }

};


class ExponentialInterpolator: public Interpolator
{
public:
    ExponentialInterpolator(double lambda=0.1): _lambda(lambda) {}

private:
    virtual double _value(double ratio) const override
    {
        return exp(_lambda - _lambda / ratio);
    }

private:
    double _lambda;

};


class InverseInterpolator: public Interpolator
{
public:
    InverseInterpolator(Interpolator *interpolator)
        : _interpolator(interpolator)
    {
        assert(interpolator);
    }
    virtual ~InverseInterpolator() override
    {
        delete _interpolator;
    }

private:
    virtual double _value(double ratio) const override
    {
        return 1 - _interpolator->value(1 - ratio);
    }

private:
    Interpolator *_interpolator;

};


}

