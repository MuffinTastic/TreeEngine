using System;
using System.Collections.Generic;
using System.Diagnostics.CodeAnalysis;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tree.Engine.Utils;

public struct TimeUntil : IEquatable<TimeUntil>
{
    private float _time;

    public float Absolute => _time;
    public float Until => _time; // TODO

    public static implicit operator float( TimeUntil timeUntil )
    {
        return timeUntil.Until;
    }

    public static implicit operator TimeUntil( float timeUntil )
    {
        return new TimeUntil()
        {
            _time = timeUntil
        };
    }

    public override bool Equals( object? other )
    {
        if ( other is TimeUntil o )
            return Equals( o );

        return false;
    }

    public bool Equals(  TimeUntil other )
    {
        return _time == other._time;
    }

    public static bool operator ==( TimeUntil a, TimeUntil b )
    {
        return a.Equals( b );
    }

    public static bool operator !=( TimeUntil a, TimeUntil b )
    {
        return !a.Equals( b );
    }

    public override int GetHashCode()
    {
        return base.GetHashCode();
    }

    public override string ToString()
    {
        return _time.ToString();
    }
}