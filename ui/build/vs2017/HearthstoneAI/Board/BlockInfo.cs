using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class BlockInfo
    {
        [DataMember]
        public int id;

        [DataMember]
        public int parent_id;

        [DataMember]
        public string type;

        [DataMember]
        public int entity_id;
    }

    class BlocksInfo : List<BlockInfo>
    {
        public bool Parse(State.Game game)
        {
            foreach (var block in game.blocks_)
            {
                this.Add(new BlockInfo()
                {
                    id = block.Value.block_id,
                    parent_id = block.Value.parent_block_id,
                    type = block.Value.block_type,
                    entity_id = block.Value.entity_id
                });
            }
            return true;
        }

        public override bool Equals(object obj)
        {
            BlocksInfo rhs = obj as BlocksInfo;
            if (rhs == null) return false;
            return this.SequenceEqual(rhs);
        }

        public override int GetHashCode()
        {
            int hash = HashHelper.init;
            foreach (var obj in this)
                HashHelper.Update(ref hash, obj);
            return hash;
        }
    }
}
