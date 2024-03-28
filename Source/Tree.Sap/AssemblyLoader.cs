using Tree.Sap.Interop;

using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;

namespace Tree.Sap;

using static ManagedHost;

public enum AssemblyLoadStatus
{
    Success, FileNotFound, FileLoadFailure, InvalidFilePath, InvalidAssembly, UnknownError
}

public static class AssemblyLoader
{
    private static readonly Dictionary<Type, AssemblyLoadStatus> s_AssemblyLoadErrorLookup = new();
    private static readonly Dictionary<int, AssemblyLoadContext?> s_AssemblyContexts = new();
    private static readonly Dictionary<int, Assembly> s_AssemblyCache = new();
    private static readonly Dictionary<int, List<GCHandle>> s_AllocatedHandles = new();
    private static AssemblyLoadStatus s_LastLoadStatus = AssemblyLoadStatus.Success;

    private static readonly AssemblyLoadContext? s_CoralAssemblyLoadContext;

    static AssemblyLoader()
    {
        s_AssemblyLoadErrorLookup.Add( typeof( BadImageFormatException ), AssemblyLoadStatus.InvalidAssembly );
        s_AssemblyLoadErrorLookup.Add( typeof( FileNotFoundException ), AssemblyLoadStatus.FileNotFound );
        s_AssemblyLoadErrorLookup.Add( typeof( FileLoadException ), AssemblyLoadStatus.FileLoadFailure );
        s_AssemblyLoadErrorLookup.Add( typeof( ArgumentNullException ), AssemblyLoadStatus.InvalidFilePath );
        s_AssemblyLoadErrorLookup.Add( typeof( ArgumentException ), AssemblyLoadStatus.InvalidFilePath );

        s_CoralAssemblyLoadContext = AssemblyLoadContext.GetLoadContext( typeof( AssemblyLoader ).Assembly );
        s_CoralAssemblyLoadContext!.Resolving += ResolveAssembly;

        CacheCoralAssemblies();
    }

    private static void CacheCoralAssemblies()
    {
        foreach ( var assembly in s_CoralAssemblyLoadContext!.Assemblies )
        {
            int assemblyId = assembly.GetName().Name!.GetHashCode();
            s_AssemblyCache.Add( assemblyId, assembly );
        }
    }

    internal static bool TryGetAssembly( int InAssemblyId, out Assembly? OutAssembly )
    {
        return s_AssemblyCache.TryGetValue( InAssemblyId, out OutAssembly );
    }

    internal static Assembly? ResolveAssembly( AssemblyLoadContext? InAssemblyLoadContext, AssemblyName InAssemblyName )
    {
        try
        {
            int assemblyId = InAssemblyName.Name!.GetHashCode();

            if ( s_AssemblyCache.TryGetValue( assemblyId, out var cachedAssembly ) )
            {
                return cachedAssembly;
            }

            foreach ( var loadContext in AssemblyLoadContext.All )
            {
                foreach ( var assembly in loadContext.Assemblies )
                {
                    if ( assembly.GetName().Name != InAssemblyName.Name )
                        continue;

                    s_AssemblyCache.Add( assemblyId, assembly );
                    return assembly;
                }
            }
        }
        catch ( Exception ex )
        {
            ManagedHost.HandleException( ex );
        }

        return null;
    }

    [UnmanagedCallersOnly]
    private static int CreateAssemblyLoadContext( NativeString InName )
    {
        string? name = InName;

        if ( name == null )
            return -1;

        var alc = new AssemblyLoadContext( name, true );
        alc.Resolving += ResolveAssembly;
        alc.Unloading += ctx =>
        {
            foreach ( var assembly in ctx.Assemblies )
            {
                var assemblyName = assembly.GetName();
                int assemblyId = assemblyName.Name!.GetHashCode();
                s_AssemblyCache.Remove( assemblyId );
            }
        };

        int contextId = name.GetHashCode();
        s_AssemblyContexts.Add( contextId, alc );
        return contextId;
    }

    [UnmanagedCallersOnly]
    private static void UnloadAssemblyLoadContext( int InContextId )
    {
        if ( !s_AssemblyContexts.TryGetValue( InContextId, out var alc ) )
        {
            LogWarning( $"Cannot unload AssemblyLoadContext '{InContextId}', it was either never loaded or already unloaded." );
            return;
        }

        if ( alc == null )
        {
            LogError( $"AssemblyLoadContext '{InContextId}' was found in dictionary but was null. This is most likely a bug." );
            return;
        }

        foreach ( var assembly in alc.Assemblies )
        {
            var assemblyName = assembly.GetName();
            int assemblyId = assemblyName.Name!.GetHashCode();

            if ( !s_AllocatedHandles.TryGetValue( assemblyId, out var handles ) )
            {
                continue;
            }

            foreach ( var handle in handles )
            {
                if ( !handle.IsAllocated || handle.Target == null )
                {
                    continue;
                }

                LogWarning( $"Found unfreed object '{handle.Target}' from assembly '{assemblyName}'. Deallocating." );
                handle.Free();
            }
        }

        ManagedObject.s_CachedMethods.Clear();

        TypeInterface.s_CachedTypes.Clear();
        TypeInterface.s_CachedMethods.Clear();
        TypeInterface.s_CachedFields.Clear();
        TypeInterface.s_CachedProperties.Clear();
        TypeInterface.s_CachedAttributes.Clear();

        s_AssemblyContexts.Remove( InContextId );
        alc.Unload();
    }

    [UnmanagedCallersOnly]
    private static int LoadAssembly( int InContextId, NativeString InAssemblyFilePath )
    {
        try
        {
            if ( string.IsNullOrEmpty( InAssemblyFilePath ) )
            {
                s_LastLoadStatus = AssemblyLoadStatus.InvalidFilePath;
                return -1;
            }

            if ( !File.Exists( InAssemblyFilePath ) )
            {
                LogError( $"Failed to load assembly '{InAssemblyFilePath}', file not found." );
                s_LastLoadStatus = AssemblyLoadStatus.FileNotFound;
                return -1;
            }

            if ( !s_AssemblyContexts.TryGetValue( InContextId, out var alc ) )
            {
                LogError( $"Failed to load assembly '{InAssemblyFilePath}', couldn't find AssemblyLoadContext with id {InContextId}." );
                s_LastLoadStatus = AssemblyLoadStatus.UnknownError;
                return -1;
            }

            if ( alc == null )
            {
                LogError( $"Failed to load assembly '{InAssemblyFilePath}', AssemblyLoadContext with id {InContextId} was null." );
                s_LastLoadStatus = AssemblyLoadStatus.UnknownError;
                return -1;
            }

            Assembly? assembly = null;

            assembly = alc.LoadFromAssemblyPath( InAssemblyFilePath! );

            LogInfo( $"Loading assembly '{InAssemblyFilePath}'" );
            var assemblyName = assembly.GetName();
            int assemblyId = assemblyName.Name!.GetHashCode();
            s_AssemblyCache.Add( assemblyId, assembly );
            s_LastLoadStatus = AssemblyLoadStatus.Success;
            return assemblyId;
        }
        catch ( Exception ex )
        {
            s_AssemblyLoadErrorLookup.TryGetValue( ex.GetType(), out s_LastLoadStatus );
            HandleException( ex );
            return -1;
        }
    }

    [UnmanagedCallersOnly]
    private static AssemblyLoadStatus GetLastLoadStatus() => s_LastLoadStatus;

    [UnmanagedCallersOnly]
    private static NativeString GetAssemblyName( int InAssemblyId )
    {
        if ( !s_AssemblyCache.TryGetValue( InAssemblyId, out var assembly ) )
        {
            LogError( $"Couldn't get assembly name for assembly '{InAssemblyId}', assembly not in dictionary." );
            return "";
        }

        var assemblyName = assembly.GetName();
        return assemblyName.Name;
    }

    internal static void RegisterHandle( Assembly InAssembly, GCHandle InHandle )
    {
        var assemblyName = InAssembly.GetName();
        int assemblyId = assemblyName.Name!.GetHashCode();

        if ( !s_AllocatedHandles.TryGetValue( assemblyId, out var handles ) )
        {
            s_AllocatedHandles.Add( assemblyId, new List<GCHandle>() );
            handles = s_AllocatedHandles[assemblyId];
        }

        handles.Add( InHandle );
    }

}
