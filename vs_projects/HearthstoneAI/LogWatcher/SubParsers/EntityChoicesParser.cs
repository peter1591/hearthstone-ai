using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace HearthstoneAI.LogWatcher.SubParsers
{
    class EntityChoicesParser
    {
        private static readonly Regex EntityChoicesCreate =
            new Regex(@"^[\s]*id=(?<id>(.*))\ Player=(?<player_entity>.*)\ TaskList=(.*)\ ChoiceType=(?<choice_type>.*)\ CountMin=(.*)\ CountMax=(.*)");
        private static readonly Regex EntityChoicesSource =
            new Regex(@"^[\s]*Source=(?<source>.*)$");
        private static readonly Regex EntityChoicesEntities =
            new Regex(@"^[\s]*Entities\[(?<idx>.*)\]=(?<entity>(.+))$");

        public EntityChoicesParser(frmMain frm, GameState game_state)
        {
            this.frm_main = frm;
            this.game_state = game_state;

            this.enumerator = this.Process().GetEnumerator();
        }

        private string parsing_log;
        private frmMain frm_main;
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
                bool rule_matched = false;

                foreach (var ret in this.ParseEntityChoiceCreate())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                this.frm_main.AddLog("[ERROR] Unhandled entity chocies log: " + this.parsing_log);
                yield return true;
            }
        }

        private IEnumerable<bool> ParseEntityChoiceCreate()
        {
            var match = EntityChoicesCreate.Match(this.parsing_log);
            if (!match.Success) yield break;

            int entity_choice_id = -1;
            if (int.TryParse(match.Groups["id"].Value.Trim(), out entity_choice_id) == false)
            {
                this.frm_main.AddLog("[INFO] Cannot get entity choice id (ignoring): " + this.parsing_log);
            }

            var player_entity_raw = match.Groups["player_entity"].Value.Trim();
            int player_entity_id = ParserUtilities.GetEntityIdFromRawString(this.game_state, player_entity_raw);

            var choice_type = match.Groups["choice_type"].Value.Trim();

            if (this.game_state.EntityChoices.ContainsKey(entity_choice_id))
            {
                this.frm_main.AddLog("[ERROR] Entity choice index overlapped (overwritting)");
                this.game_state.EntityChoices.Remove(entity_choice_id);
            }

            this.game_state.EntityChoices[entity_choice_id] = new GameState.EntityChoice();
            this.game_state.EntityChoices[entity_choice_id].id = entity_choice_id;
            this.game_state.EntityChoices[entity_choice_id].choice_type = choice_type;
            this.game_state.EntityChoices[entity_choice_id].player_entity_id = player_entity_id;
            this.game_state.EntityChoices[entity_choice_id].choices_has_sent = false;

            yield return true;

            while (true)
            {
                if (!ParseEntityChoiceSource(entity_choice_id)) break;
                yield return true;
            }

            while (true)
            {
                if (ParseEntityChoiceEntity(entity_choice_id)) yield return true;
                else break;
            }
        }

        private bool ParseEntityChoiceSource(int entity_choice_id)
        {
            var match = EntityChoicesSource.Match(this.parsing_log);
            if (!match.Success) return false;
            this.game_state.EntityChoices[entity_choice_id].source = match.Groups["source"].Value.Trim();
            return true;
        }

        private bool ParseEntityChoiceEntity(int entity_choice_id)
        {
            if (!EntityChoicesEntities.IsMatch(this.parsing_log)) return false;

            var match = EntityChoicesEntities.Match(this.parsing_log);

            if (this.game_state.EntityChoices.ContainsKey(entity_choice_id) == false)
            {
                this.frm_main.AddLog("[ERROR] missing current entity choice id");
                return false;
            }

            int idx;
            if (int.TryParse(match.Groups["idx"].Value.Trim(), out idx) == false)
            {
                this.frm_main.AddLog("[ERROR] Parse failed: " + this.parsing_log);
                return false;
            }

            var entity_raw = match.Groups["entity"].Value.Trim();
            int entity_id = ParserUtilities.GetEntityIdFromRawString(this.game_state, entity_raw);
            if (entity_id < 0)
            {
                this.frm_main.AddLog("[ERROR] Failed to get entity id: " + this.parsing_log);
                return false;
            }

            this.game_state.EntityChoices[entity_choice_id].choices[idx] = entity_id;

            // Trigger
            this.EntityChoiceAdded(entity_choice_id);
            return true;
        }

        private void EntityChoiceAdded(int entity_choice_id)
        {
            var choice = this.game_state.EntityChoices[entity_choice_id];
            if (choice.choice_type == "MULLIGAN")
            {
                this.DeterminePlayerAndOpponent(choice);
            }
        }

        private void DeterminePlayerAndOpponent(GameState.EntityChoice mulligan)
        {
            // if mulligan choice have card id, then it's the player
            // otherwise, it's the opponent

            if (mulligan.choices.Count == 0) return;
            var choice = mulligan.choices[0];

            if (this.game_state.PlayerEntityId < 0 && this.game_state.Entities[choice].CardId != "")
            {
                this.game_state.PlayerEntityId = mulligan.player_entity_id;
            }

            if (this.game_state.OpponentEntityId < 0 && this.game_state.Entities[choice].CardId == "")
            {
                this.game_state.OpponentEntityId = mulligan.player_entity_id;
            }
        }
    }
}
