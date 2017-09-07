using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace HearthstoneAI.LogWatcher.SubParsers
{
    class DebugPrintPowerParser
    {
        public delegate void LogMsgDelegate(String msg);
        public LogMsgDelegate log_msg;

        private static readonly Regex TagChangeRegex =
            new Regex(@"^[\s]*TAG_CHANGE\ Entity=(?<entity>(.+))\ tag=(?<tag>(\w+))\ value=(?<value>(\w+))");

        public DebugPrintPowerParser(GameState game_state)
        {
            this.game_state = game_state;
            this.enumerator = this.Process().GetEnumerator();
        }

        private string parsing_log;
        private GameState game_state;
        private IEnumerator<bool> enumerator;

        public void Process(string log)
        {
            this.parsing_log = log;
            this.enumerator.MoveNext();
        }

        private IEnumerable<bool> Process()
        {
            while (true)
            {
                bool matched = false;
                foreach (var ret in this.ParseTagChange())
                {
                    matched = true;
                    yield return ret;
                }

                if (!matched)
                {
                    yield return true; // ignore all non-parsed items
                }
            }
        }

        private IEnumerable<bool> ParseTagChange()
        {
            var match = TagChangeRegex.Match(this.parsing_log);
            if (!match.Success) yield break;


            var entity_raw = match.Groups["entity"].Value;
            int entityId = ParserUtilities.GetEntityIdFromRawString(this.game_state, entity_raw);

            var tag = GameState.Entity.ParseTag(match.Groups["tag"].Value, match.Groups["value"].Value);

            PatchPlayerName(entity_raw, tag.Item1, tag.Item2);

            yield return true;
        }

        private void PatchPlayerName(String entity_id, GameTag tag, int tag_value)
        {
            if (tag == GameTag.PLAYSTATE && tag_value == (int)TAG_PLAYSTATE.PLAYING)
            {
                // the player's name can only be known from debug print power logs
                // so we parse the name out, and write back to our entity object
                foreach (var entity in this.game_state.Entities)
                {
                    foreach (var entity_tag in entity.Value.Tags)
                    {
                        if (entity_tag.Key == GameTag.PLAYSTATE && entity_tag.Value == (int)TAG_PLAYSTATE.PLAYING)
                        {
                            if (entity.Value.Name == String.Empty)
                            {
                                entity.Value.Name = entity_id;
                                return;
                            }
                        }
                    }
                }

                log_msg("Failed to patch player name.");
            }
        }
    }
}
