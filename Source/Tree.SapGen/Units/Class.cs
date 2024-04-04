using System.Collections.Generic;

namespace Tree.SapGen;

public class Class : IUnit
{
	public Class( string name )
	{
		Name = name;

		Methods = new();
		Bases = new(); 
	}

	public string Name { get; set; }
	public List<Method> Methods { get; set; }
    public int Count { get => Methods.Count; }
    public List<Class> Bases { get; set; }
	public bool IsNamespace { get; set; }

	public override string ToString()
	{
		return Name;
	}

    public long GetCompileHash()
    {
        return (long) Name.GetHashCode() + IsNamespace.GetHashCode()
            + Methods.Select( m => (long) m.GetHashCode() ).Sum()
            + Bases.Select( b => (long) b.GetHashCode() ).Sum();
    }
}
