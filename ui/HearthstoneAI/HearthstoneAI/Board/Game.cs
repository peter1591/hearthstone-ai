using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class Game
    {
        [DataMember]
        public Player player = new Player();

        [DataMember]
        public Player opponent = new Player();

        public bool Parse(GameState game)
        {
            bool ret = true;

            GameState.Entity player_entity;
            if (game.TryGetPlayerEntity(out player_entity) == false) ret = false;
            else
            {
                ret = this.player.Parse(game, player_entity) && ret;
            }

            GameState.Entity opponent_entity;
            if (game.TryGetOpponentEntity(out opponent_entity) == false) ret = false;
            else
            {
                ret = this.opponent.Parse(game, opponent_entity) && ret;
            }

            return ret;
        }
    }
}
