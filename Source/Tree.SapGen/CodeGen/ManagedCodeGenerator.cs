using System.Collections.Generic;
using System.CodeDom.Compiler;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Xml.Linq;

namespace Tree.SapGen;

sealed class ManagedCodeGenerator : BaseCodeGenerator
{
	public ManagedCodeGenerator( List<IUnit> units ) : base( units )
	{
    }

	public static string GenerateManagedHashCode( long hash )
    {
        var (baseTextWriter, writer) = Utils.CreateWriter();

		writer.WriteLine( GetHeader() );
		writer.WriteLine();
		writer.WriteLine( $"namespace {GetNamespace()};" );
        writer.WriteLine();
        writer.WriteLine( "internal static unsafe class SapHash" );
		writer.WriteLine( "{" );
		writer.Indent++;
        {
            writer.WriteLine( $"private static delegate* unmanaged< long > {BuildDelegateIdent( "GetNativeSapHash" )} = default;" );
            writer.WriteLine();
            writer.WriteLine( $"private const long ManagedHash = {hash};" );
            writer.WriteLine();
            writer.WriteLine( $"internal static bool Verify()" );
            writer.WriteLine( "{" );
            writer.Indent++;
			{
				writer.WriteLine( $"return ( {BuildDelegateIdent( "GetNativeSapHash" )}(  ) == ManagedHash );" );
			}
			writer.Indent--;
            writer.WriteLine( "}" );
        }
		writer.Indent--;
		writer.WriteLine( "}" );

        return baseTextWriter.ToString();
    }

    public string GenerateManagedCode()
    {
        var (baseTextWriter, writer) = Utils.CreateWriter();

		var allStructs = Units.OfType<Structure>().Select( s => s.Name ).ToHashSet();

        writer.WriteLine( GetHeader() );
        writer.WriteLine();

        foreach ( var usingStatement in GetUsings() )
            writer.WriteLine( $"using {usingStatement};" );

        writer.WriteLine();
        writer.WriteLine( $"namespace {GetNamespace()};" );
        writer.WriteLine();

        foreach ( var unit in Units )
        {
            if ( unit is Class c )
            {
                GenerateMethodCode( ref writer, c );
            }

            if ( unit is Structure s )
            {
                GenerateStructCode( ref writer, allStructs, s );
            }

			if ( unit is Enum e )
			{
				GenerateEnumCode( ref writer, e );
			}

            writer.WriteLine();
        }

        return baseTextWriter.ToString();
    }

    private List<string> GetUsings()
	{
		return new() {
            "System.Runtime.CompilerServices",
            "System.Runtime.InteropServices",
            "Tree.Sap.Interop"
		};
	}

	public static string GetNamespace()
	{
		return "Tree.Sap.Generated";
	}

	private static string GetDefaultAccessLevel()
	{
		return "internal";
	}

	private void GenerateMethodCode( ref IndentedTextWriter writer, Class c )
	{
		//
		// Gather everything we need into nice lists
		//
		List<string> nativeDelegates = new();

		foreach ( var method in c.Methods )
        {
            bool hasInstance = !method.IsStatic && !c.IsNamespace;

            var returnType = Utils.GetManagedInternalTypeSub( method.ReturnType );
			var name = method.Name;

			var returnsPointer = Utils.IsPointer( method.ReturnType ) && !method.IsConstructor && !method.IsDestructor;

			if ( returnsPointer )
				returnType = "nint";

            if ( method.IsConstructor || method.IsDestructor )
				returnType = "nint"; // Ctor/dtor handled specially too

			var parameterTypes = method.Parameters
				.Select( p => Utils.GetManagedInternalTypeSub( p.Type ) )
				.ToList(); 
			var paramAndReturnTypes = parameterTypes.Append( returnType );

			if ( hasInstance )
				paramAndReturnTypes = paramAndReturnTypes.Prepend( "nint" ); // Pointer to this class's instance

			var delegateTypeArguments = string.Join( ", ", paramAndReturnTypes );

			//
			// This gets set by Sap
			//
			nativeDelegates.Add( $"private static delegate* unmanaged< {delegateTypeArguments} > {BuildDelegateIdent( name )} = default;" );
		}

		//
		// Write class definition
		//
		string classAccessLevel = GetDefaultAccessLevel();
		if ( c.IsNamespace )
			classAccessLevel += " static";

		string classDefinition = $"{classAccessLevel} unsafe class {c.Name}";

		if ( !c.IsNamespace )
			classDefinition += $" : INativeSap";

        writer.WriteLine( classDefinition );
		writer.WriteLine( "{" );
		writer.Indent++;

		{
			//
			// Native instance pointer
			//

			if ( !c.IsNamespace ) 
			{
				writer.WriteLine( $"public nint NativePtr {{ get; set; }}" );
				writer.WriteLine();
			}

			// Delegates/native function pointers
			foreach ( var @delegate in nativeDelegates )
			{
				writer.WriteLine( @delegate );
			}

			// Ctor
			if ( c.Methods.Any( x => x.IsConstructor ) && !c.IsNamespace )
			{
				var ctor = c.Methods.First( x => x.IsConstructor );
				var managedCtorArgs = string.Join( ", ", ctor.Parameters.Select( x => $"{Utils.GetManagedUserTypeSub( x.Type )} {x.Name}" ) );

                writer.WriteLine();

                writer.WriteLine( $"{GetDefaultAccessLevel()} {c.Name}( {managedCtorArgs} )" );
				writer.WriteLine( "{" );
				writer.Indent++;

				var ctorCallArgs = string.Join( ", ", ctor.Parameters.Select( x => x.Name ) );
				writer.WriteLine( $"this.NativePtr = this.Ctor( {ctorCallArgs} );" );

				writer.Indent--;
				writer.WriteLine( "}" );
			}

			// Methods
			foreach ( var method in c.Methods )
            {
                bool hasInstance = !method.IsStatic && !c.IsNamespace;

                writer.WriteLine();

				//
				// Gather function signature
				//
				// Call parameters as comma-separated string
				var managedCallParams = string.Join( ", ", method.Parameters.Select( x => $"{Utils.GetManagedUserTypeSub( x.Type )} {x.Name}" ) );

				// We return a pointer to the created object if it's a ctor/dtor, but otherwise we'll do auto-conversions to our managed types
				var returnType = ( method.IsConstructor || method.IsDestructor ) ? "nint" : Utils.GetManagedUserTypeSub( method.ReturnType );

				var returnsPointer = Utils.IsPointer( method.ReturnType ) && !method.IsConstructor && !method.IsDestructor;

				// If this is a ctor or dtor, we don't want to be able to call the method manually
				var methodAccessLevel = ( method.IsConstructor || method.IsDestructor ) ? "private" : GetDefaultAccessLevel();

				if ( !hasInstance )
					methodAccessLevel += " static";

				// Write function signature
				writer.WriteLine( $"{methodAccessLevel} {returnType} {method.Name}( {managedCallParams} ) " );
				writer.WriteLine( "{" );
				writer.Indent++;

				//
				// Gather function body
				//
				var args = method.Parameters;

				// We need to pass the instance in if this is not a static method
				if ( hasInstance )
					args = args.Prepend( new Variable( "NativePtr", "nint" ) ).ToList();

				BuildVariables( ref writer, args );
				var @params = FixCallParams( args );

                // Function call arguments as comma-separated string
                var functionCall = $"{BuildDelegateIdent( method.Name )}( {@params} )";

                if ( returnsPointer )
				{
					// If we want to return a pointer:
					writer.WriteLine( $"nint ptr = {functionCall};" );
					writer.WriteLine( $"var obj = RuntimeHelpers.GetUninitializedObject( typeof( {returnType} ) ) as {returnType};" );
					writer.WriteLine( $"obj.NativePtr = ptr;" );
					writer.WriteLine( $"return obj;" );
				}
                else
				{
                    if ( returnType == "void" )
                    {
                        writer.WriteLine( $"{functionCall};" );
                    }
					else
                    {
                        writer.WriteLine( $"return {functionCall};" );
                    }
				}

				writer.Indent--;
				writer.WriteLine( "}" );
			}
		}

		writer.Indent--;
		writer.WriteLine( "}" );
	}

	public static string BuildDelegateIdent( string name )
	{
		return $"_del_{name}";
	}

	private void BuildVariables( ref IndentedTextWriter writer, List<Variable> args )
    {
        foreach ( var arg in args )
        {
            if ( Utils.TypeIsString( arg.Type ) )
            {
                writer.WriteLine( $"using NativeString _sap_{arg.Name} = {arg.Name};" );
            }
        }
    }
    private string FixCallParams( List<Variable> @params )
    {
        return string.Join( ", ", @params.Select( p =>
        {
            if ( Utils.TypeIsString( p.Type ) )
            {
                return $"_sap_{p.Name}";
            }
            else
            {
                return p.Name;
            }
        } ) );
    }

    private void GenerateStructCode( ref IndentedTextWriter writer, HashSet<string> allStructs, Structure s )
    {
		writer.WriteLine( $"[StructLayout( LayoutKind.Sequential )]" );
        writer.WriteLine( $"{GetDefaultAccessLevel()} struct {s.Name} : IDisposable" );
        writer.WriteLine( "{" );
        writer.Indent++;

		{
			foreach ( var field in s.Fields )
			{
				writer.WriteLine( $"{GetDefaultAccessLevel()} {Utils.GetManagedInternalTypeSub( field.Type )} {field.Name};" );
			}


            var disposables = s.Fields.Where( f =>
			{
				var sub = Utils.GetManagedInternalTypeSub( f.Type );
				return sub == "NativeString" || Utils.TypeIsArray( sub ) || allStructs.Contains( sub );
			} ).ToList();

            writer.WriteLine();
            writer.WriteLine( "public void Dispose()" );
			writer.WriteLine( "{" );
			writer.Indent++;

			foreach ( var field in disposables )
			{
				writer.WriteLine( $"{field.Name}.Dispose();" );
			}

			{
				writer.WriteLine( "GC.SuppressFinalize( this );" );
			}
			writer.Indent--;
            writer.WriteLine( "}" );
		}

        writer.Indent--;
        writer.WriteLine( "}" );
    }

    private void GenerateEnumCode( ref IndentedTextWriter writer, Enum e )
    {
        writer.WriteLine( $"{GetDefaultAccessLevel()} enum {e.Name} : int" );
        writer.WriteLine( "{" );
        writer.Indent++;

        foreach ( var constant in e.Constants)
        {
			writer.WriteLine( $"{constant.Name} = {constant.Value}," );
        }

        writer.Indent--;
        writer.WriteLine( "}" );
    }
}
