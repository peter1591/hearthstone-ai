using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Communicator
{
    [DataContract]
    class ActionGetCurrentBestMove : ActionBase
    {
        public ActionGetCurrentBestMove(int sequence)
            : base(sequence, "GetCurrentBestMove")
        {
        }
    }
}
