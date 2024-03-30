using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Tree.SapGen;

public static class Program
{
    private const string c_NativeOutputRel = "Tree.Root/sapgen";
    private const string c_ManagedOutputRel = "Tree.Engine/SapGen";

    private static List<IUnit> s_Units { get; set; } = new();
    private static List<string> s_NativeHeaders { get; set; } = new();

    public static void Main()
    {
        string sourceDir = Directory.GetCurrentDirectory();

        if ( !Directory.Exists( Path.Join( sourceDir, "Tree.Root" ) ) )
        {
            Console.WriteLine( "Not in Source directory, exiting" );
            return;
        }

        Console.WriteLine( $"--- Generating C++/C# sap for {sourceDir} ---" );
        var startTime = DateTime.Now;

        GenerateSap( sourceDir );
        GenerateFinalFiles( sourceDir );

        var endTime = DateTime.Now;
        var duration = endTime - startTime;
        Console.WriteLine( $"--- Sap done. Took {duration.TotalSeconds} seconds ---" );
    }

    private static void GenerateSap( string sourcePath )
    {
        DeleteExistingSap( sourcePath );

        ParseCode( sourcePath );
    }

    private static void DeleteExistingSap( string sourcePath )
    {
        string nativeOutPath = Path.Join( sourcePath, c_NativeOutputRel );
        string managedOutPath = Path.Join( sourcePath, c_ManagedOutputRel );

        if ( Directory.Exists( nativeOutPath ) )
            Directory.Delete( nativeOutPath, true );
        if ( Directory.Exists( managedOutPath ) )
            Directory.Delete( managedOutPath, true );

        Directory.CreateDirectory( nativeOutPath );
        Directory.CreateDirectory( managedOutPath );
    }

    private static void ParseCode( string sourcePath )
    {
        List<string> queue = new();

        QueueHeaders( ref queue, sourcePath );

        if ( queue.Count == 0 )
            return;

        var dispatcher = new ThreadDispatcher<string>(
            ( files ) =>
            {
                foreach ( var file in files )
                {
                    ProcessHeader( sourcePath, file );
                }
            },
            queue
            );

        while ( !dispatcher.IsComplete )
            Thread.Sleep( 100 );
    }

    private static void QueueHeaders( ref List<string> queue, string baseDir )
    {
        foreach ( var file in Directory.GetFiles( baseDir ) )
        {
            if ( file.EndsWith(".h") && !file.EndsWith( ".sap.h" ) )
            {
                var contents = File.ReadAllText( file );

                if ( contents.Contains( "SAP_GEN_SKIP", StringComparison.CurrentCultureIgnoreCase ) )
                    continue;
                if ( !contents.Contains( "SAP_GEN", StringComparison.CurrentCultureIgnoreCase ) )
                    continue;

                queue.Add( file );
            }
        }

        foreach ( var subDir in Directory.GetDirectories( baseDir ) )
        {
            QueueHeaders( ref queue, subDir );
        }
    }

    private static void ProcessHeader( string sourcePath, string path )
    {
        Console.WriteLine( path );

        var fileName = Path.GetFileNameWithoutExtension( path );
        string nativeOutPath = Path.Join( sourcePath, c_NativeOutputRel, $"{fileName}.sap.h" );
        string managedOutPath = Path.Join( sourcePath, c_ManagedOutputRel, $"{fileName}.sap.cs" );

        var units = Parser.GetUnits( path );

        var nativeGenerator = new NativeCodeGenerator( units );
        var relativePath = Path.GetRelativePath( sourcePath, path );
        var nativeCode = nativeGenerator.GenerateNativeCode( relativePath );
        Console.WriteLine( $"Native Code at '{nativeOutPath}':" );
        Console.WriteLine( nativeCode );
        File.WriteAllText( nativeOutPath, nativeCode );

        var managedGenerator = new ManagedCodeGenerator( units );
        var managedCode = managedGenerator.GenerateManagedCode();
        Console.WriteLine( "Managed Code:" );
        Console.WriteLine( managedCode );
        File.WriteAllText( managedOutPath, managedCode );

        var sapHeaderPath = Path.GetRelativePath( sourcePath, nativeOutPath );
        s_NativeHeaders.Add( sapHeaderPath );
        s_Units.AddRange( units );
    }

    private static void GenerateFinalFiles( string sourcePath )
    {
        string nativeOutPath = Path.Join( sourcePath, c_NativeOutputRel, "upload.h" );

        var methods = s_Units.OfType<Class>()
            .SelectMany( unit => unit.Methods, ( unit, method ) => (unit.Name, method) )
            .ToList();

        var nativeCode = NativeCodeGenerator.GenerateNativeUploadCode( s_NativeHeaders, methods );

        Console.WriteLine( "Upload code: " );
        Console.WriteLine( nativeCode );
        File.WriteAllText( nativeOutPath, nativeCode );
    }
}
