using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using Tree.Engine;
using Tree.Sap.Generated;
using Tree.Sap.Interop;

namespace Tree.Trunk;

public class SapEntry
{


    private static Bool32 Startup()
    {
        if ( !SapHash.Verify() )
        {
            Log.Error( "Sap hash failed verification" );
            return false;
        }

        return true;
    }

    private static void Shutdown()
    {

    }
}