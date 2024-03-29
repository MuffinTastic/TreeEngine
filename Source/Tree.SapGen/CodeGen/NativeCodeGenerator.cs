using System.Collections.Generic;
using System.CodeDom.Compiler;
using System.Linq;

namespace Tree.SapGen;

sealed class NativeCodeGenerator : BaseCodeGenerator
{
	public NativeCodeGenerator( List<IUnit> units ) : base( units )
	{
	}

	public string GenerateNativeCode( string headerPath )
	{
		var (baseTextWriter, writer) = Utils.CreateWriter();

		writer.WriteLine( GetHeader() );
		writer.WriteLine();

		writer.WriteLine( "#pragma once" );
		writer.WriteLine( "#include \"Tree.Root/sap/String.h\"" );
        writer.WriteLine( "#include \"Tree.Root/sap/Array.h\"" );
        writer.WriteLine( $"#include \"{headerPath}\"" );
		writer.WriteLine();
		writer.WriteLine( "using namespace Tree;" );
        writer.WriteLine();

        foreach ( var unit in Units )
		{
			if ( unit is Class c )
			{
				if ( c.IsNamespace )
					GenerateNamespaceCode( ref writer, c );
				else
					GenerateClassCode( ref writer, c );
			}

			writer.WriteLine();
		}

		return baseTextWriter.ToString();
	}

	private void GenerateNamespaceCode( ref IndentedTextWriter writer, Class c )
	{
		foreach ( var method in c.Methods )
		{
			var args = method.Parameters;

			var argStr = string.Join( ", ", args.Select( x =>
			{
				if ( x.Type == "std::string" )
				{
					return $"const char* {x.Name}";
				}

				return $"{x.Type} {x.Name}";
			} ) );

			var signature = $"extern \"C\" inline {method.ReturnType} __{c.Name}_{method.Name}( {argStr} )";
			var body = "";
			var @params = string.Join( ", ", method.Parameters.Select( x => x.Name ) );

			var accessor = $"{c.Name}::";

			if ( method.ReturnType == "void" )
				body += $"{accessor}{method.Name}( {@params} );";
			else if ( method.ReturnType == "std::string" )
				body += $"std::string text = {accessor}{method.Name}( {@params} );\r\nconst char* cstr = text.c_str();\r\nchar* dup = _strdup(cstr);\r\nreturn dup;";
			else
				body += $"return {accessor}{method.Name}( {@params} );";

			writer.WriteLine( signature );
			writer.WriteLine( "{" );
			writer.Indent++;

			writer.WriteLine( body );

			writer.Indent--;
			writer.WriteLine( "}" );
		}
	}

	private void GenerateClassCode( ref IndentedTextWriter writer, Class c )
	{
		foreach ( var method in c.Methods )
		{
			var args = method.Parameters;

			if ( !method.IsStatic )
				args = args.Prepend( new Variable( "instance", $"{c.Name}*" ) ).ToList();

			var argStr = string.Join( ", ", args.Select( x =>
			{
				string sub = Utils.GetNativeTypeSub( x.Type );

				return $"{sub} {x.Name}";
			} ) );

			var signature = $"extern \"C\" inline {method.ReturnType} __{c.Name}_{method.Name}( {argStr} )";
			var body = "";
			var @params = string.Join( ", ", method.Parameters.Select( x => x.Name ) );

			if ( method.IsConstructor )
			{
				body += $"return new {c.Name}( {@params} );";
			}
			else if ( method.IsDestructor )
			{
				body += $"instance->~{c.Name}( {@params} );";
			}
			else
			{
				var accessor = method.IsStatic ? $"{c.Name}::" : "instance->";

				if ( method.ReturnType == "void" )
				{
					body += $"{accessor}{method.Name}( {@params} );";
				}
				else
				{
					body += $"auto val = {accessor}{method.Name}( {@params} );";

					if ( Utils.NativeTypeIsString( method.ReturnType ) )
					{
						body += "return Sap::String::New( val )";
					}
					else
					{
						body += "return val;";
					}
				}
			}

			writer.WriteLine( signature );
			writer.WriteLine( "{" );
			writer.Indent++;

			writer.WriteLine( body );

			writer.Indent--;
			writer.WriteLine( "}" );
		}
	}
}
