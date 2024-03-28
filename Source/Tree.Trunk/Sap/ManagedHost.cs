using Tree.Sap.Interop;

using System;
using System.Runtime.InteropServices;

namespace Tree.Sap;

internal enum MessageLevel { Info = 1, Warning = 2, Error = 4 }

internal static class ManagedHost
{
	private static unsafe delegate*<NativeString, void> s_ExceptionCallback;
    private static unsafe delegate*< NativeString, void > s_InfoCallback;
    private static unsafe delegate*< NativeString, void > s_WarningCallback;
    private static unsafe delegate*< NativeString, void > s_ErrorCallback;

    [UnmanagedCallersOnly]
    private static unsafe void Initialize(
        delegate*<NativeString, void> InExceptionCallback,
        delegate*<NativeString, void> InInfoCallback )
    {
        s_ExceptionCallback = InExceptionCallback;
    }

    internal static void HandleException( Exception InException )
    {
        unsafe
        {
            if ( s_ExceptionCallback == null )
                return;

            using NativeString message = InException.ToString();
            s_ExceptionCallback( message );
        }
    }

    internal static void LogInfo( string InMessage )
    {
        unsafe
        {
            using NativeString message = InMessage;
            s_InfoCallback( message );
        }
    }

    internal static void LogWarning( string InMessage )
    {
        unsafe
        {
            using NativeString message = InMessage;
            s_WarningCallback( message );
        }
    }

    internal static void LogError( string InMessage )
    {
        unsafe
        {
            using NativeString message = InMessage;
            s_ErrorCallback( message );
        }
    }
}
