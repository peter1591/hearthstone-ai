using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Communicator
{
    [DataContract]
    class ActionBoardActionStart : ActionBase
    {
        [DataMember]
        Board.Game game;

        public ActionBoardActionStart(int sequence, Board.Game game)
            : base(sequence, "BoardActionStart")
        {
            this.game = game;
        }
    }
}
