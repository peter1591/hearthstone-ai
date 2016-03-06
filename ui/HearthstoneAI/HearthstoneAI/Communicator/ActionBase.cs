using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Communicator
{
    [DataContract]
    abstract class ActionBase
    {
        [DataMember]
        public int sequence;

        [DataMember]
        public string action;

        protected ActionBase(int sequence, string action)
        {
            this.sequence = sequence;
            this.action = action;
        }
    }
}
