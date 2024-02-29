using Antlr4.Runtime;
using Antlr4.Runtime.Tree;
using System;
using System.Diagnostics.CodeAnalysis;
using System.Linq;

namespace Tree.GlueGen
{
    internal class DEFListener : DEFBaseListener
    {
        private Parser _parser;

        public DEFListener( Parser parser )
        {
            _parser = parser;
        }

        public override void EnterDef( [NotNull] DEFParser.DefContext context )
        {

        }

        public override void ExitDef( [NotNull] DEFParser.DefContext context )
        {

        }
        public override void EnterInteropDefinition( [NotNull] DEFParser.InteropDefinitionContext context )
        {

        }

        public override void ExitInteropDefinition( [NotNull] DEFParser.InteropDefinitionContext context )
        {
            Console.WriteLine( "}" );
        }

        public override void EnterStructDefinition( [NotNull] DEFParser.StructDefinitionContext context )
        {
            Console.WriteLine( $"struct {context.identifier().GetText()} {{" );
        }

        public override void ExitStructDefinition( [NotNull] DEFParser.StructDefinitionContext context )
        {

        }

        public override void EnterSystemDefinition( [NotNull] DEFParser.SystemDefinitionContext context )
        {
            bool managed = context.managed() is not null;
            Console.WriteLine( $"system {context.identifier().GetText()} ({(managed ? "c#" : "c++" )}) {{" );
        }

        public override void ExitSystemDefinition( [NotNull] DEFParser.SystemDefinitionContext context )
        {

        }

        /*
        public override void EnterInteropDefinition( [NotNull] DEFParser.InteropDefinitionContext context )
        {
            var type = context.interopTypeSpecifier().GetChild( 0 ).GetType();

            switch ( type )
            {

            }

            var identifier = context.identifier().GetText();

            Console.WriteLine( $"{type} {identifier} {{" );
        }

        public override void ExitInteropDefinition( [NotNull] DEFParser.InteropDefinitionContext context )
        {
            Console.WriteLine( "}" );
        }

        public override void EnterInteropItem( [NotNull] DEFParser.InteropItemContext context )
        {
            Console.WriteLine( $"    {context.GetChild( 0 ).GetType().Name}: {context.GetChild( 0 ).GetChild( 0 ).GetText()}" );
        }


        public override void ExitInteropItem( [NotNull] DEFParser.InteropItemContext context )
        {

        }*/

        public override void EnterVariable( [NotNull] DEFParser.VariableContext context )
        {
            var typ = context.typeSpecifier();
            Console.WriteLine( typ );
            Console.WriteLine( $"{context.typeSpecifier().GetText()} {context.identifier().GetText()};");
        }

        public override void ExitVariable( [NotNull] DEFParser.VariableContext context )
        {

        }

        public override void EnterFunction( [NotNull] DEFParser.FunctionContext context )
        {
            Console.Write( $"{context.typeSpecifier().GetText()} {context.identifier().GetText()}(" );

            if ( context.funcArgumentList() is DEFParser.FuncArgumentListContext list )
            {
                Console.WriteLine();

                var arguments = list.children.OfType<DEFParser.FuncArgumentContext>();

                foreach ( var arg in arguments )
                {

                    Console.WriteLine( $"   {arg.typeSpecifier().GetText()} {arg.identifier().GetText()}" );
                }
            }

            Console.WriteLine( ")" );
        }

        public override void ExitFunction( [NotNull] DEFParser.FunctionContext context )
        {

        }

        public override void EnterFuncArgumentList( [NotNull] DEFParser.FuncArgumentListContext context )
        {

        }

        public override void ExitFuncArgumentList( [NotNull] DEFParser.FuncArgumentListContext context )
        {

        }

        public override void EnterFuncArgument( [NotNull] DEFParser.FuncArgumentContext context )
        {

        }

        public override void ExitFuncArgument( [NotNull] DEFParser.FuncArgumentContext context )
        {

        }

        public override void EnterIdentifier( [NotNull] DEFParser.IdentifierContext context )
        {

        }

        public override void ExitIdentifier( [NotNull] DEFParser.IdentifierContext context )
        {

        }

        public override void EnterTypeSpecifier( [NotNull] DEFParser.TypeSpecifierContext context )
        {

        }

        public override void ExitTypeSpecifier( [NotNull] DEFParser.TypeSpecifierContext context )
        {

        }

        public override void EnterPointer( [NotNull] DEFParser.PointerContext context )
        {

        }

        public override void ExitPointer( [NotNull] DEFParser.PointerContext context )
        {

        }
    }
}
