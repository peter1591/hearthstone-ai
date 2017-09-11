using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.State
{
    public class JoustInformation
    {
        public JoustInformation() { this.entities = new List<int>(); }

        public int AddEntityId(int entity_id)
        {
            if (this.entities.Contains(entity_id)) return 0;
            this.entities.Add(entity_id);
            return 1;
        }

        public readonly List<int> entities;
    }
}
