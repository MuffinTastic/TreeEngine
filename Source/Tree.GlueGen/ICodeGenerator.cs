namespace Tree.GlueGen
{
    enum InteropType
    {
        System,
        Struct
    }

    internal interface ICodeGenerator
    {
        void EnterRootDef();
        void ExitRootDef();

        void EnterInteropDef( InteropType type, string identifer );
        void ExitInteropDef();

        string Output();
    }
}
