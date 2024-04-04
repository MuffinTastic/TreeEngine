using System.Collections.Generic;

namespace Tree.SapGen;

public class Method
{
	public Method( string name, string returnType )
	{
		Name = name;
		ReturnType = returnType;
		Parameters = new();
	}

	public bool IsConstructor { get; set; } = false;
	public bool IsDestructor { get; set; } = false;
	public bool IsStatic { get; set; } = false;
	public bool IsConverter { get; set; } = false;

	public string Name { get; set; }
	public string ReturnType { get; set; }
	public List<Variable> Parameters { get; set; }

	public override string ToString()
	{
		var p = string.Join( ", ", Parameters );
		return $"{ReturnType} {Name}( {p} )";
	}

    public long GetCompileHash()
    {
		return (long) IsConstructor.GetHashCode() + IsDestructor.GetHashCode() + IsStatic.GetHashCode()
			+ IsConverter.GetHashCode() + Name.GetHashCode() + ReturnType.GetHashCode()
			+ Parameters.Select( p => (long) p.GetHashCode() ).Sum();
    }
}
