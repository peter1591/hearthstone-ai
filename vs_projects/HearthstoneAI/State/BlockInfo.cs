using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.State
{
    class BlockInfo
    {
        public int block_id;
        public int parent_block_id; // -1 if no parent
        public string block_type;
        public int entity_id;
    }
}
