using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tree.Engine.Utils;

internal class Stopwatch : IDisposable
{
    private string _name;
    private TimeSince _startTime;

    public Stopwatch( string name )
    {
        _name = name;
        _startTime = 0.0f;
    }

    public void Dispose()
    {
        Log.Info( $"{_name} took {_startTime} seconds" );
    }
}
