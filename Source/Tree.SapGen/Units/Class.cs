using System.Collections.Generic;

namespace Tree.SapGen;

public struct Class : IUnit
{
	public Class( string name ) : this()
	{
		Name = name;

		Fields = new();
		Methods = new();
		Bases = new(); 
	}

	public string Name { get; set; }
	public List<Method> Methods { get; set; }
	public List<Variable> Fields { get; set; }
	public List<Class> Bases { get; set; }
	public bool IsNamespace { get; set; }

	public override string ToString()
	{
		return Name;
	}
}
