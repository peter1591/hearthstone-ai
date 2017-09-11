using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace HearthstoneAI.LogWatcher.SubParsers
{
    class SendChoicesParser : BaseParser
    {
        private static readonly Regex SendChoices =
            new Regex(@"^[\s]*id=(?<id>(.*))\ ChoiceType=(?<choice_type>.*)");
        private static readonly Regex SendChoicesEntities =
            new Regex(@"^[\s]*m_chosenEntities\[(?<idx>.*)\]=(?<entity>(.+))$");

        public SendChoicesParser(Game game_state, Logger logger)
            : base(logger)
        {
            this.game_state = game_state;
            this.enumerator = this.Process().GetEnumerator();
        }

        private string parsing_log;
        private Game game_state;
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
                foreach (var ret in this.ParseOneItem()) yield return ret;
            }
        }

        private IEnumerable<bool> ParseOneItem()
        {
            int entity_choice_id = -1;
            while (true)
            {
                if (!SendChoices.IsMatch(this.parsing_log))
                {
                    logger_.Info("[INFO] Waiting for send-choice log, but got " + this.parsing_log + " (ignoring)");
                    continue;
                }
                var match = SendChoices.Match(this.parsing_log);
                entity_choice_id = Int32.Parse(match.Groups["id"].Value.Trim());

                this.game_state.EntityChoices[entity_choice_id].choices_has_sent = true;
                yield return true;
                break;
            }

            while (true)
            {
                if (SendChoicesEntities.IsMatch(this.parsing_log))
                {
                    var match_chosen_entities = SendChoicesEntities.Match(this.parsing_log);
                    var chosen_entity = match_chosen_entities.Groups["entity"].Value.Trim();
                    var chosen_entity_id = ParserUtilities.GetEntityIdFromRawString(this.game_state, chosen_entity);
                    if (chosen_entity_id < 0)
                    {
                        logger_.Info("[ERROR] Failed to get entity id for a choice: " + this.parsing_log + " (ignoring)");
                        yield return true;
                        continue;
                    }

                    this.game_state.EntityChoices[entity_choice_id].sent_choices.Add(chosen_entity_id);
                    yield return true;
                }
                else break;
            }

        }
    }
}
