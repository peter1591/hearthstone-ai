using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace HearthstoneAI.LogWatcher.SubParsers
{
    class DebugPrintOptions : BaseParser
    {
        public DebugPrintOptions(Logger logger) : base(logger) { }

        public class StartWaitingMainActionEventArgs : EventArgs { }
        public event EventHandler<StartWaitingMainActionEventArgs> StartWaitingMainAction;

        public void Process(string log)
        {
            if (log.StartsWith("id="))
            {
                StartWaitingMainAction(this, new StartWaitingMainActionEventArgs());
            }
        }
    }
}
