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

        [DataMember]
        public int turn;

        public bool Parse(State.Game game)
        {
            bool ret = true;

            State.Game.Entity game_entity;
            if (!game.TryGetGameEntity(out game_entity)) ret = false;
            else
            {
                this.turn = game_entity.GetTagOrDefault(State.GameTag.TURN, 0);
            }

            State.Game.Entity player_entity;
            if (!game.TryGetPlayerEntity(out player_entity)) ret = false;
            else
            {
                ret = this.player.Parse(game, player_entity) && ret;
            }

            State.Game.Entity opponent_entity;
            if (!game.TryGetOpponentEntity(out opponent_entity)) ret = false;
            else
            {
                ret = this.opponent.Parse(game, opponent_entity) && ret;
            }

            return ret;
        }

        public override bool Equals(object obj)
        {
            Game rhs = obj as Game;
            if (rhs == null) return false;
            if (!this.player.Equals(rhs.player)) return false;
            if (!this.opponent.Equals(rhs.opponent)) return false;
            if (!this.turn.Equals(rhs.turn)) return false;
            return true;
        }

        public override int GetHashCode()
        {
            return new { this.player, this.opponent, this.turn }.GetHashCode();
        }
    }
}
