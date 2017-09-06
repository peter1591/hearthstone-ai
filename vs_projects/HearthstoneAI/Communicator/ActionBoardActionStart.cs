using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Communicator
{
    [DataContract]
    class ActionUpdateBoard : ActionBase
    {
        [DataMember]
        Board.Game game;

        public ActionUpdateBoard(int sequence, Board.Game game)
            : base(sequence, "UpdateBoard")
        {
            this.game = game;
        }
    }
}
