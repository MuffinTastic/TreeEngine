namespace Tree.SapGen;

public class Variable
{
	public Variable( string name, string type )
	{
		Name = name;
		Type = type;
	}

	public string Name { get; set; }
	public string Type { get; set; }

	public override string ToString()
	{
		return $"{Type} {Name}";
	}

    public long GetCompileHash()
    {
		return (long) Name.GetHashCode() + Type.GetHashCode();
    }
}
