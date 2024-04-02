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
}
