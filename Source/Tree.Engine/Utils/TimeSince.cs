using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tree.Engine.Utils;

public struct TimeSince : IEquatable<TimeSince>
{
    private float _time;

    public float Absolute => _time;
    public float Since => _time; // TODO

    public static implicit operator float( TimeSince timeSince )
    {
        return timeSince.Since; // TODO
    }

    public static implicit operator TimeSince( float timeSince )
    {
        return new TimeSince()
        {
            _time = timeSince
        };
    }

    public override bool Equals( object? other )
    {
        if ( other is TimeSince o )
            return Equals( o );

        return false;
    }

    public bool Equals( TimeSince other )
    {
        return _time == other._time;
    }

    public static bool operator ==( TimeSince a, TimeSince b )
    {
        return a.Equals( b );
    }

    public static bool operator !=( TimeSince a, TimeSince b )
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