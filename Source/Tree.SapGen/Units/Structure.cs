﻿using System.Collections.Generic;

namespace Tree.SapGen;

public class Structure : IUnit
{
	public Structure( string name )
	{
		Name = name;

		Fields = new();
	}

	public string Name { get; set; }
	public List<Variable> Fields { get; set; }
	public int Count { get => Fields.Count; }

	public override string ToString()
	{
		return Name;
	}

    public long GetCompileHash()
    {
		return (long) Name.GetHashCode()
			+ Fields.Select( f => (long) f.GetHashCode() ).Sum();
    }
}
