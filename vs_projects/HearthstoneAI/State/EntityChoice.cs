using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.State
{
    public class EntityChoice
    {
        public EntityChoice()
        {
            choices = new Dictionary<int, int>();
            choices_has_sent = false;
            sent_choices = new List<int>();
        }

        public int id;
        public string choice_type;
        public int player_entity_id;
        public string player_entity_str;
        public string source;
        public Dictionary<int, int> choices;

        public bool choices_has_sent;
        public List<int> sent_choices;
    }
}
