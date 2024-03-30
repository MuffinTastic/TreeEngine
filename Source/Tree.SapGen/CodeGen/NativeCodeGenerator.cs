using System.Collections.Generic;
using System.CodeDom.Compiler;
using System.Linq;
using System;

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
        writer.WriteLine();
        writer.WriteLine( "#include \"Tree.Root/sap/String.h\"" );
        writer.WriteLine( "#include \"Tree.Root/sap/Array.h\"" );
        writer.WriteLine();
        writer.WriteLine( $"#include \"{headerPath}\"" );
		writer.WriteLine();
		writer.WriteLine( "using namespace Tree;" );

        foreach ( var unit in Units )
		{
			if ( unit is Class c )
			{
				GenerateFunctionCode( ref writer, c );
			}

			writer.WriteLine();
		}

		return baseTextWriter.ToString();
	}

	private void GenerateFunctionCode( ref IndentedTextWriter writer, Class c )
	{
		foreach ( var method in c.Methods )
        {
            bool hasInstance = !method.IsStatic && !c.IsNamespace;

            var args = method.Parameters;

            if ( hasInstance )
                args = args.Prepend( new Variable( "instance", $"{c.Name}*" ) ).ToList();

            writer.WriteLine();

            //
            // Set up signature
            //
            BuildSignature( ref writer, c, method, args );

            //
            // Write function body
            //

            writer.WriteLine( "{" );
            writer.Indent++;

			{
				BuildVariables( ref writer, args );

				var @params = FixCallParams( method.Parameters );

				if ( hasInstance && method.IsConstructor )
				{
                    writer.WriteLine( $"return new {c.Name}( {@params} );" );
				}
				else if ( hasInstance && method.IsDestructor )
				{
                    writer.WriteLine( $"instance->~{c.Name}( {@params} );" );
				}
				else
				{
					var accessor = hasInstance ? "instance->" : $"{c.Name}::";

					if ( method.ReturnType == "void" )
					{
                        writer.WriteLine( $"{accessor}{method.Name}( {@params} );" );
					}
					else
					{
                        writer.WriteLine( $"auto val = {accessor}{method.Name}( {@params} );" );

						if ( Utils.TypeIsString( method.ReturnType ) )
						{
                            writer.WriteLine( "return Sap::String::New( val );" );
						}
						else
						{
                            writer.WriteLine( "return val;" );
						}
					}
				}
            }

            writer.Indent--;
            writer.WriteLine( "}" );
        }
	}

	private void BuildSignature( ref IndentedTextWriter writer, Class c, Method method, List<Variable> args )
    {
        var returnType = Utils.GetNativeTypeSub( method.ReturnType );
        var argStr = string.Join( ", ", args.Select( x => $"{Utils.GetNativeTypeSub( x.Type )} {x.Name}" ) );

        var signature = $"extern \"C\" inline {returnType} __{c.Name}_{method.Name}( {argStr} )";

        writer.WriteLine( signature );
    }

    private void BuildVariables( ref IndentedTextWriter writer, List<Variable> args )
    {
        foreach ( var arg in args )
        {
            if ( Utils.TypeIsString( arg.Type ) )
            {
                writer.WriteLine( $"std::string _sap_{arg.Name} = std::string({arg.Name});" );
            }
        }
    }

	private string FixCallParams( List<Variable> @params )
    {
        return string.Join( ", ", @params.Select( p =>
        {
            if ( Utils.TypeIsCharString( p.Type ) )
            {
                return $"_sap_{p.Name}.c_str()";
            }
            else if ( Utils.TypeIsStdString( p.Type ) )
            {
                return $"std::move(_sap_{p.Name})";
            }
            else
            {
                return p.Name;
            }
        } ) );
    }
}
