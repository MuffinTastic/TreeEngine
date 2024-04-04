using System.Collections.Generic;

namespace Tree.SapGen;

public interface IUnit
{
	public string Name { get; set; }
	public int Count { get; }

	public long GetCompileHash();
}
