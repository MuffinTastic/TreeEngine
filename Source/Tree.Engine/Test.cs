using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Tree.Sap.Interop;

namespace Tree.Engine;

public class EngineTest
{
    internal static unsafe delegate*< NativeString, void > TestFunc;

    public void StringDemo()
    {
        using NativeString str = "Hello, World?";
        unsafe { TestFunc( str ); }
    }
}
