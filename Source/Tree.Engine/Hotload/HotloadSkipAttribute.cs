using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tree.Engine.Hotload;

[AttributeUsage( AttributeTargets.Property | AttributeTargets.Field, Inherited = false )]
public sealed class HotloadSkipAttribute : Attribute
{
    public HotloadSkipAttribute()
    {

    }
}