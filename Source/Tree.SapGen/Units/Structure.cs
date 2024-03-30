using System.Collections.Generic;

namespace Tree.SapGen;

public class Structure : IUnit
{
	public Structure( string name )
	{
		Name = name;

		Fields = new();
		Methods = new();
	}

	public string Name { get; set; }
	public List<Method> Methods { get; set; }
	public List<Variable> Fields { get; set; }

	public override string ToString()
	{
		return Name;
	}
}
