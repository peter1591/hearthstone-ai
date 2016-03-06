using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Communicator
{
    [DataContract]
    class ActionCancel : ActionBase
    {
        public ActionCancel(int sequence)
            : base(sequence, "Cancel")
        {
        }
    }
}
