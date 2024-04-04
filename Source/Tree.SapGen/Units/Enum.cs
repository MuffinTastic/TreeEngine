using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tree.SapGen;

public class Enum : IUnit
{
    public Enum( string name )
    {
        Name = name;

        Constants = new();
    }

    public string Name { get; set; }
    public List<EnumConstant> Constants { get; set; }
    public int Count { get => Constants.Count; }

    public long GetCompileHash()
    {
        return Name.GetHashCode()
            + Constants.Select( c => (long) c.GetHashCode() ).Sum();
    }
}
