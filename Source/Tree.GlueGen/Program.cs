using System;
using System.IO;
using Antlr4.Runtime;
using Antlr4.Runtime.Tree;

namespace Tree.GlueGen
{
    class Program
    {
        static void Main( string[] args )
        {
            var program = new Program();
            program.Run();
        }

        ParseTreeWalker walker = new ParseTreeWalker();

        private void Run()
        {
            string currentDir = Directory.GetCurrentDirectory();
            string[] files = Directory.GetFiles( $"{currentDir}\\bindings", "*.def" );
            
            foreach ( string file in files )
            {
                Generate( file );
            }
        }

        private void Generate( string file )
        {
            var inputStream = new AntlrFileStream( file );
            var lexer = new DEFLexer( inputStream );
            var tokenStream = new CommonTokenStream( lexer );
            var parser = new DEFParser( tokenStream );

            var errorListener = new DefErrorListener();

            lexer.RemoveErrorListeners();
            lexer.AddErrorListener( errorListener );
            parser.RemoveErrorListeners();
            parser.AddErrorListener( errorListener );

            
            var listener = new DEFListener( parser );
            walker.Walk( listener, parser.def() );

            if ( errorListener.Errored )
            {
                Console.WriteLine( $"'{file}' errored" );
            }

        }
    }

    public class DefErrorListener : BaseErrorListener, IAntlrErrorListener<int>
    {
        public bool Errored { get; private set; } = false;

        // Lexer
        public void SyntaxError(
            TextWriter output, IRecognizer recognizer,
            int offendingSymbol, int line,
            int charPositionInLine, string msg,
            RecognitionException e )
        {
            string sourceName = recognizer.InputStream.SourceName;
            Console.WriteLine( "line:{0} col:{1} src:{2} msg:{3}", line, charPositionInLine, sourceName, msg );

            Errored = true;
        }

        // Parser
        public override void SyntaxError(
            TextWriter output, IRecognizer recognizer,
            IToken offendingSymbol, int line,
            int charPositionInLine, string msg,
            RecognitionException e )
        {
            string sourceName = recognizer.InputStream.SourceName;
            Console.WriteLine( "line:{0} col:{1} src:{2}\n  : {3}", line, charPositionInLine, sourceName, msg );

            Errored = true;
        }
    }
}