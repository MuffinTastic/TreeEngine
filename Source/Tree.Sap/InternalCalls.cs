using System;
using System.Linq;
using System.Runtime.InteropServices;
using System.Reflection;

namespace Tree.Sap.Interop;

using static ManagedHost;

[StructLayout( LayoutKind.Sequential, Pack = 1 )]
public readonly struct InternalCall
{
    private readonly IntPtr m_NamePtr;
    public readonly IntPtr NativeFunctionPtr;

    public string? Name => Marshal.PtrToStringAuto( m_NamePtr );
}

internal static class InternalCallsManager
{
    [UnmanagedCallersOnly]
    private static void SetInternalCalls( IntPtr InInternalCalls, int InLength )
    {
        var internalCalls = new NativeArray<InternalCall>( InInternalCalls, InLength );

        try
        {
            for ( int i = 0; i < internalCalls.Length; i++ )
            {
                var internalCall = internalCalls[i];
                var name = internalCall.Name;

                if ( name == null )
                {
                    LogError( $"Cannot register internal at index '{i}' call with null name!" );
                    continue;
                }

                var fieldNameStart = name.IndexOf( '+' );
                var fieldNameEnd = name.IndexOf( ",", fieldNameStart, StringComparison.CurrentCulture );
                var fieldName = name.Substring( fieldNameStart + 1, fieldNameEnd - fieldNameStart - 1 );
                var containingTypeName = name.Remove( fieldNameStart, fieldNameEnd - fieldNameStart );

                var type = TypeInterface.FindType( containingTypeName );

                if ( type == null )
                {
                    LogError( $"Cannot register internal call '{name}', failed to type '{containingTypeName}'." );
                    continue;
                }

                var bindingFlags = BindingFlags.Static | BindingFlags.NonPublic;
                var field = type.GetFields( bindingFlags ).FirstOrDefault( field => field.Name == fieldName );

                if ( field == null )
                {
                    LogError( $"Cannot register internal '{name}', failed to find it in type '{containingTypeName}'" );
                    continue;
                }

                if ( !field.FieldType.IsFunctionPointer )
                {
                    LogError( $"Field '{name}' is not a function pointer type!" );
                    continue;
                }

                field.SetValue( null, internalCall.NativeFunctionPtr );
            }
        }
        catch ( Exception ex )
        {
            HandleException( ex );
        }
    }
}
