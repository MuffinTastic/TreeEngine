using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

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

    private static HashSet<string> s_NativeCharStringSet = new HashSet<string>
        { "char*", "char *" };

    private static HashSet<string> s_NativeStdStringSet = new HashSet<string>
        { "std::string", "std::string_view" };

    private static Dictionary<string, string> s_ManagedUserTypeSubTable = new Dictionary<string, string>()
        {
			// Native type		Substitute type
			//-------------------------------
			{ "void",               "void" },
            { "uint32_t",           "uint" },
            { "int32_t",            "int" },
            { "uint64_t",           "ulong" },
            { "int64_t",            "long" },
            { "size_t",             "ulong" },

            { "char**",             "ref string" },
            { "char **",            "ref string" },
            { "char*",              "string" },
            { "char *",             "string" },
            { "std::string",        "string" },
            { "std::string_view",   "string" },

            { "void*",              "nint" },
            { "void *",             "nint" },

            // It's up to Tree.Engine to handle how to expose these arrays
            { "Sap::Array",         "NativeArray" },

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

    private static Dictionary<string, string> s_ManagedInternalTypeSubTable = new Dictionary<string, string>()
        {
			// Native type		Substitute type
			//-------------------------------
			{ "void",               "void" },
            { "uint32_t",           "uint" },
            { "int32_t",            "int" },
            { "uint64_t",           "ulong" },
            { "int64_t",            "long" },
            { "size_t",             "ulong" },

            { "char**",             "NativeString*" },
            { "char **",            "NativeString*" },
            { "char*",              "NativeString" },
            { "char *",             "NativeString" },
            { "std::string",        "NativeString" },
            { "std::string_view",   "NativeString" },
            { "Sap::String",        "NativeString" },

            { "void*",              "nint" },
            { "void *",             "nint" },

            { "Sap::Array",         "NativeArray" },

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

    private static Regex s_TemplateRegex = new Regex( @"^(.*?)<(.*)>$" );

    public static bool IsPointer( string nativeType )
	{
		var managedType = GetManagedUserTypeSub( nativeType );
		return nativeType.Trim().EndsWith( "*" ) && managedType != "string" && managedType != "nint";
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

    public static bool TypeIsCharString( string nativeType )
    {
        nativeType = nativeType.Trim();

        if ( nativeType.StartsWith( "const" ) )
            nativeType = nativeType[5..].Trim();

        // Check if the native type is in the lookup table
        if ( s_NativeCharStringSet.Contains( nativeType ) )
        {
            return true;
        }

        return false;
    }

    public static bool TypeIsStdString( string nativeType )
    {
        nativeType = nativeType.Trim();

        if ( nativeType.StartsWith( "const" ) )
            nativeType = nativeType[5..].Trim();

        // Check if the native type is in the lookup table
        if ( s_NativeStdStringSet.Contains( nativeType ) )
        {
            return true;
        }

        return false;
    }

    public static bool TypeIsString( string nativeType )
    {
        return TypeIsCharString( nativeType ) || TypeIsStdString( nativeType );
    }

    public static bool TypeIsArray( string nativeType )
    {
        if ( nativeType.Contains( "<" ) && nativeType.EndsWith( ">" ) )
        {
            Match match = s_TemplateRegex.Match( nativeType );
            string templateClass = GetManagedInternalTypeSub( match.Groups[1].Value );
            return templateClass == "NativeArray";
        }

        return false;
    }

    public static string GetManagedUserTypeSub( string nativeType )
	{
		// Trim whitespace from beginning / end (if it exists)
		nativeType = nativeType.Trim();

		// Remove the "const" keyword
		if ( nativeType.StartsWith( "const" ) )
			nativeType = nativeType[5..].Trim();

		// Check if the native type is a reference
		if ( nativeType.EndsWith( "&" ) )
			return GetManagedUserTypeSub( nativeType[0..^1] );

		// Check if the native type is in the lookup table
		if ( s_ManagedUserTypeSubTable.ContainsKey( nativeType ) )
		{
			return s_ManagedUserTypeSubTable[nativeType];
        }

        if ( nativeType.Contains( "<" ) && nativeType.EndsWith( ">" ) )
        {
            Match match = s_TemplateRegex.Match( nativeType );
            string templateClass = GetManagedUserTypeSub( match.Groups[1].Value );
            string templateType = GetManagedUserTypeSub( match.Groups[2].Value );
            return $"{templateClass}<{templateType}>";
        }

        // Check if the native type is a pointer
        if ( nativeType.EndsWith( "*" ) )
			return GetManagedUserTypeSub( nativeType[..^1].Trim() ); // We'll return the basic type, because we handle pointers on the C# side now

		// Return the native type if it is not in the lookup table
		return nativeType;
    }

    public static string GetManagedInternalTypeSub( string nativeType )
    {
        // Trim whitespace from beginning / end (if it exists)
        nativeType = nativeType.Trim();

        // Remove the "const" keyword
        if ( nativeType.StartsWith( "const" ) )
            nativeType = nativeType[5..].Trim();

        // Check if the native type is a reference
        if ( nativeType.EndsWith( "&" ) )
            return GetManagedInternalTypeSub( nativeType[0..^1] );

        // Check if the native type is in the lookup table
        if ( s_ManagedInternalTypeSubTable.ContainsKey( nativeType ) )
        {
            return s_ManagedInternalTypeSubTable[nativeType];
        }

        if ( nativeType.Contains( "<" ) && nativeType.EndsWith( ">" ) )
        {
            Match match = s_TemplateRegex.Match( nativeType );
            string templateClass = GetManagedInternalTypeSub( match.Groups[1].Value );
            string templateType = GetManagedInternalTypeSub( match.Groups[2].Value );
            return $"{templateClass}<{templateType}>";
        }

        // Check if the native type is a pointer
        if ( nativeType.EndsWith( "*" ) )
            return GetManagedInternalTypeSub( nativeType[..^1].Trim() ); // We'll return the basic type, because we handle pointers on the C# side now

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
