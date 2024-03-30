using System.Collections.Generic;
using System.CodeDom.Compiler;
using System.Linq;
using System;
using System.IO;

namespace Tree.SapGen;

sealed class NativeCodeGenerator : BaseCodeGenerator
{
	public NativeCodeGenerator( List<IUnit> units ) : base( units )
	{
	}

    public static string GetNamespace()
    {
        return "Tree::Sap::Generated";
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
		writer.WriteLine( $"namespace {GetNamespace()}" );
        writer.WriteLine( "{" );
        writer.Indent++;

        foreach ( var unit in Units )
		{
			if ( unit is Class c )
			{
				GenerateFunctionCode( ref writer, c );
			}

			writer.WriteLine();
        }

        writer.Indent--;
        writer.WriteLine( "}" );

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

                if ( hasInstance && method.IsConverter )
                {
                    writer.WriteLine( $"return dynamic_cast<{method.ReturnType}>( instance );" );
                }
				else if ( hasInstance && method.IsConstructor )
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

    public static string BuildFunctionIdent( string className, string methodName)
    {
        return $"__{className}_{methodName}";
    }

	private void BuildSignature( ref IndentedTextWriter writer, Class c, Method method, List<Variable> args )
    {
        var returnType = Utils.GetNativeTypeSub( method.ReturnType );
        var argStr = string.Join( ", ", args.Select( x => $"{Utils.GetNativeTypeSub( x.Type )} {x.Name}" ) );

        var identifier = BuildFunctionIdent( c.Name, method.Name );
        var signature = $"inline {returnType} {identifier}( {argStr} )";

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

    public static string GenerateNativeUploadCode( List<string> includes, List<(string className, Method method)> methods )
    {
        var (baseWriter, writer) = Utils.CreateWriter();

        var managedNamespace = ManagedCodeGenerator.GetNamespace();

        writer.WriteLine( GetHeader() );
        writer.WriteLine();
        writer.WriteLine( "// !!! NOTE: This should only be included by ManagedHostSystem !!!" );
        writer.WriteLine();
        writer.WriteLine( "#pragma once" );
        writer.WriteLine();
        writer.WriteLine( "#include \"Tree.Root/sap/Assembly.h\"" );
        writer.WriteLine();

        foreach ( var header in includes )
        {
            writer.WriteLine( $"#include \"{header}\"" );
        }

        writer.WriteLine();

        writer.WriteLine( $"namespace {GetNamespace()}" );
        writer.WriteLine( "{" );
        writer.Indent++;

        {
            writer.WriteLine( "inline void AddSapCalls( Tree::Sap::ManagedAssembly& assembly )" );
            writer.WriteLine( "{" );
            writer.Indent++;

            foreach ( (string className, Method method) in methods )
            {
                string managedClassName = $"{managedNamespace}.{className}";
                string managedDelIdent = ManagedCodeGenerator.BuildDelegateIdent( method.Name );
                string nativeFuncIdent = BuildFunctionIdent( className, method.Name );
                writer.WriteLine( $"assembly.AddInternalCall(\"{managedClassName}\", \"{managedDelIdent}\", reinterpret_cast<void*>( &{nativeFuncIdent} ) );" );
            }

            writer.Indent--;
            writer.WriteLine( "}" );
        }

        writer.Indent--;
        writer.WriteLine( "}" );

        writer.Dispose();
        return baseWriter.ToString();
    }
}
