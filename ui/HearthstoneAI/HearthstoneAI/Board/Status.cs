using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class Status
    {
        [DataMember]
        public int charge;

        [DataMember]
        public int taunt;

        [DataMember]
        public int divine_shield;

        [DataMember]
        public int stealth;

        [DataMember]
        public int forgetful;

        [DataMember]
        public int freeze;

        [DataMember]
        public int frozen;

        [DataMember]
        public int poisonous;

        [DataMember]
        public int windfury;

        public bool Parse(GameState game, GameState.Entity entity)
        {
            bool ret = true;
            ret = this.ParseOneStatus(entity, GameTag.CHARGE, out this.charge) && ret;
            ret = this.ParseOneStatus(entity, GameTag.TAUNT, out this.taunt) && ret;
            ret = this.ParseOneStatus(entity, GameTag.DIVINE_SHIELD, out this.divine_shield) && ret;
            ret = this.ParseOneStatus(entity, GameTag.STEALTH, out this.stealth) && ret;
            ret = this.ParseOneStatus(entity, GameTag.FORGETFUL, out this.forgetful) && ret;
            ret = this.ParseOneStatus(entity, GameTag.FREEZE, out this.freeze) && ret;
            ret = this.ParseOneStatus(entity, GameTag.FROZEN, out this.frozen) && ret;
            ret = this.ParseOneStatus(entity, GameTag.POISONOUS, out this.poisonous) && ret;
            ret = this.ParseOneStatus(entity, GameTag.WINDFURY, out this.windfury) && ret;

            return ret;
        }

        private bool ParseOneStatus(GameState.Entity entity, GameTag tag, out int value)
        {
            value = entity.GetTagOrDefault(tag, 0);

            return true;
        }
    }
}
