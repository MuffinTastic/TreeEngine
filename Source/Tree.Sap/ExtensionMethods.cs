using System;

namespace Tree.Sap;

public static class ExtensionMethods
{
    public static bool IsDelegate( this Type InType )
    {
        return typeof( MulticastDelegate ).IsAssignableFrom( InType.BaseType );
    }
}
