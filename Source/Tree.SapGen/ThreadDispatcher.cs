using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Tree.SapGen;

public class ThreadDispatcher<T>
{
    public delegate void ThreadCallback( List<T> threadQueue );

    private int m_ThreadCount = 8;
    private int m_ThreadsCompleted = 0;
    public bool IsComplete => m_ThreadsCompleted == m_ThreadCount;

    public ThreadDispatcher( ThreadCallback threadStart, List<T> queue )
    {
        var batchSize = queue.Count / m_ThreadCount;
        var remainder = queue.Count % m_ThreadCount;

        if ( remainder != 0 )
            batchSize++;

        if ( batchSize == 0 )
            return; // Bail to avoid division by zero

        var batched = queue
            .Select( ( Value, Index ) => new { Value, Index } )
            .GroupBy( p => p.Index / batchSize )
            .Select( g => g.Select( p => p.Value ).ToList() )
            .ToList();

        if ( batched.Count < m_ThreadCount )
            m_ThreadCount = batched.Count; // Min. 1 per thread

        for ( int i = 0; i < batched.Count; i++ )
        {
            var threadQueue = batched[i];
            var thread = new Thread( () =>
            {
                threadStart( threadQueue );

                m_ThreadsCompleted++;
            } );

            thread.Start();
        }
    }
}
