using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;

namespace Tree.SapGen;

static class Utils
{
	private static Dictionary<string, string> s_NativeTypeSubTable = new Dictionary<string, string>()
        {
			// Native type		Substitute type
			//-------------------------------
            { "char*",              "Sap::String" },
            { "char *",             "Sap::String" },
            { "std::string",        "Sap::String" },
            { "std::string_view",   "Sap::String" },
        };

    private static Dictionary<string, string> s_ManagedTypeSubTable = new Dictionary<string, string>()
        {
			// Native type		Substitute type
			//-------------------------------
			{ "void",               "void" },
            { "uint32_t",           "uint" },
            { "int32_t",            "int" },
            { "size_t",             "uint" },

            { "char**",             "ref string" },
            { "char **",            "ref string" },
            { "char*",              "string" },
            { "char *",             "string" },
            { "std::string",        "NativeString" },
            { "std::string_view",   "NativeString" },

            { "void*",              "nint" },
            { "void *",             "nint" },

			// GLM
			{ "glm::vec2",          "Vector2" },
            { "glm::vec3",          "Vector3" },
            { "glm::mat4",          "Matrix4x4" },
            { "glm::quat",          "Rotation" },

			// Custom
			{ "Quaternion",         "Rotation" },
            { "InteropStruct",      "IInteropArray" },
            { "Handle",             "uint" }
        };


    public static bool IsPointer( string nativeType )
	{
		var managedType = GetManagedTypeSub( nativeType );
		return nativeType.Trim().EndsWith( "*" ) && managedType != "string" && managedType != "IntPtr";
    }

    public static string GetNativeTypeSub( string nativeType )
    {
        nativeType = nativeType.Trim();

        if ( nativeType.StartsWith( "const" ) )
            nativeType = nativeType[5..].Trim();

        // Check if the native type is in the lookup table
        if ( s_NativeTypeSubTable.ContainsKey( nativeType ) )
        {
            return s_NativeTypeSubTable[nativeType];
        }

        // Return the native type if it is not in the lookup table
        return nativeType;
    }

	public static bool NativeTypeIsString( string nativeType )
    {
        nativeType = nativeType.Trim();

        if ( nativeType.StartsWith( "const" ) )
            nativeType = nativeType[5..].Trim();

        // Check if the native type is in the lookup table
        if ( s_NativeTypeSubTable.ContainsKey( nativeType ) )
        {
            return true;
        }

        return false;
    }

    public static string GetManagedTypeSub( string nativeType )
	{
		// Trim whitespace from beginning / end (if it exists)
		nativeType = nativeType.Trim();

		// Remove the "const" keyword
		if ( nativeType.StartsWith( "const" ) )
			nativeType = nativeType[5..].Trim();

		// Check if the native type is a reference
		if ( nativeType.EndsWith( "&" ) )
			return GetManagedTypeSub( nativeType[0..^1] );

		// Check if the native type is in the lookup table
		if ( s_ManagedTypeSubTable.ContainsKey( nativeType ) )
		{
			return s_ManagedTypeSubTable[nativeType];
		}

		// Check if the native type is a pointer
		if ( nativeType.EndsWith( "*" ) )
			return GetManagedTypeSub( nativeType[..^1].Trim() ); // We'll return the basic type, because we handle pointers on the C# side now

		// Return the native type if it is not in the lookup table
		return nativeType;
	}

	public static (StringWriter StringWriter, IndentedTextWriter TextWriter) CreateWriter()
	{
		var baseTextWriter = new StringWriter();

		var writer = new IndentedTextWriter( baseTextWriter, "    " )
		{
			Indent = 0
		};

		return (baseTextWriter, writer);
	}

	public static void EnsureLibClang()
    {
#if WINDOWS
		string sourceDir = Directory.GetCurrentDirectory();
        string toolLibraries = Path.Join( Directory.GetParent( sourceDir )!.FullName, "ThirdParty", "ToolBin" );

        string exeFilePath = System.Reflection.Assembly.GetExecutingAssembly().Location;
		string exeDir = Path.GetDirectoryName( exeFilePath )!;

		string clangOldLocation = Path.Join( toolLibraries, "libclang.dll" );
        string clangNewLocation = Path.Join( exeDir, "libclang.dll" );

		if ( !File.Exists( clangNewLocation ) )
			File.Copy( clangOldLocation, clangNewLocation );
#endif
    }
}
