using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Starter
{
    class Program
    {
        static void Main(string[] args)
        {
            string clientName = "BrontosaurusGore_";
            string serverName = "TServer_Applictaion_x64_";
#if DEBUG
            clientName += "Debug";
            serverName += "Debug";
#else
            clientName += "Release";
            serverName += "Release";
#endif
            clientName += ".exe";
            serverName += ".exe";

            Process.Start(serverName);
            Thread.Sleep(1000);
            Process.Start(clientName);
        }
    }
}
