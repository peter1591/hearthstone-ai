using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Communicator
{
    [DataContract]
    class ActionGetBestMove : ActionBase
    {
        [DataMember]
        Board.Game game;

        public ActionGetBestMove(int sequence, Board.Game game)
            : base(sequence, "GetBestMove")
        {
            this.game = game;
        }
    }
}
