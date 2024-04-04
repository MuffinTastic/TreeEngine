using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Tree.SapGen;

public class EnumConstant
{
    public EnumConstant( string name, string value )
    {
        Name = name;
        Value = value;
    }

    public string Name { get; set; }
    public string Value { get; set; }

    public long GetCompileHash()
    {
        return (long) Name.GetHashCode() + Value.GetHashCode();
    }
}