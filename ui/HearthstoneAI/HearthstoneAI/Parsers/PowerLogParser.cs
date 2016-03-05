using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace HearthstoneAI.Parsers
{
    class PowerLogParser
    {
        private static readonly Regex CreateGameRegex = new Regex(@"^[\s]*CREATE_GAME");

        private static readonly Regex GameEntityRegex = new Regex(@"^[\s]*GameEntity\ EntityID=(?<id>(\d+))");

        private static readonly Regex PlayerEntityRegex =
            new Regex(@"^[\s]*Player\ EntityID=(?<id>(\d+))\ PlayerID=(?<playerId>(\d+))\ GameAccountId=(?<gameAccountId>(.+))");

        private static readonly Regex CardIdRegex = new Regex(@"cardId=(?<cardId>(\w+))");

        private static readonly Regex FullEntityRegex = new Regex(@"^[\s]*FULL_ENTITY - Creating[\s]+ID=(?<id>(\d+))[\s]+CardID=(?<cardId>(\w*))");
        private static readonly Regex CreationTagRegex = new Regex(@"^[\s]*tag=(?<tag>(\w+))\ value=(?<value>(\w+))");

        private static readonly Regex TagChangeRegex =
            new Regex(@"^[\s]*TAG_CHANGE\ Entity=(?<entity>(.+))\ tag=(?<tag>(\w+))\ value=(?<value>(\w+))");

        private static readonly Regex ShowEntityRegex =
            new Regex(@"^[\s]*SHOW_ENTITY\ -\ Updating\ Entity=(?<entity>(.+))\ CardID=(?<cardId>(\w*))");

        private static readonly Regex HideEntityRegex =
            new Regex(@"^[\s]*HIDE_ENTITY\ -\ Entity=(?<entity>(.+))\ tag=(?<tag>(\w+))\ value=(?<value>(\w+))");

        private static readonly Regex ActionStartRegex =
            new Regex(@"^(?<indent>[\s]*)ACTION_START[\s]+Entity=(?<entity>(.*))[\s]+BlockType=(?<block_type>.*)[\s+]Index=(?<index>.*)[\s]+Target=(?<target>.*)");
        private static readonly Regex ActionEndRegex = new Regex(@"^[\s]*ACTION_END");

        private static readonly Regex MetadataRegex = new Regex(@"^[\s]*META_DATA\ -\ ");
        private static readonly Regex MetadataInfoRegex = new Regex(@"^[\s]*Info\[\d+\]");

        private List<GameState.Entity> tmp_entities = new List<GameState.Entity>();

        public PowerLogParser(frmMain frm, GameState game_state)
        {
            this.frm_main = frm;
            this.game_state = game_state;

            this.enumerator = this.Process().GetEnumerator();
        }

        public void Process(string log_line)
        {
            this.parsing_log = log_line;
            this.enumerator.MoveNext();
        }

        private string parsing_log;
        private frmMain frm_main;
        private GameState game_state;
        private IEnumerator<bool> enumerator;

        private IEnumerable<bool> Process()
        {
            while (true)
            {
                bool rule_matched = false;

                foreach (var ret in this.ParseCreateGame())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                this.frm_main.AddLog("[ERROR] Parse power log failed: " + this.parsing_log);
                yield return true;
            }
        }

        private IEnumerable<bool> ParseCreateGame()
        {
            if (!CreateGameRegex.IsMatch(this.parsing_log)) yield break;
            yield return true;

            // a CREATE_GAME will present for every 30 minutes
            // or maybe when you re-connected to the game?
            // So here we don't reset the game unless the Game State is COMPLETE
            if (this.game_state.GameEntityId > 0)
            {
                var game_entity = this.game_state.Entities[this.game_state.GameEntityId];
                if (game_entity.GetTagOrDefault(GameTag.STATE, (int)TAG_STATE.RUNNING) == (int)TAG_STATE.COMPLETE)
                {
                    this.game_state.Reset();
                }
            }
            //this.game_state.Reset();

            if (!GameEntityRegex.IsMatch(this.parsing_log))
            {
                this.frm_main.AddLog("[ERROR] Expecting the game entity log.");
                yield break;
            }

            var game_entity_match = GameEntityRegex.Match(this.parsing_log);
            var game_entity_id = int.Parse(game_entity_match.Groups["id"].Value);
            this.game_state.CreateGameEntity(game_entity_id);
            yield return true;

            while (true)
            {
                if (this.ParseCreatingTag(game_entity_id)) yield return true;
                else break;
            }

            for (int i = 0; i < 2; i++) // two players
            {
                if (!PlayerEntityRegex.IsMatch(this.parsing_log))
                {
                    this.frm_main.AddLog("[ERROR] Expecting the player entity log.");
                    yield break;
                }

                var match = PlayerEntityRegex.Match(this.parsing_log);
                var id = int.Parse(match.Groups["id"].Value);
                if (!this.game_state.Entities.ContainsKey(id))
                    this.game_state.Entities.Add(id, new GameState.Entity(id));
                yield return true;

                while (true)
                {
                    if (this.ParseCreatingTag(id)) yield return true;
                    else break;
                }
            }

            while (true)
            {
                bool rule_matched = false;

                foreach (var ret in this.ParseFullEntity())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                if (this.ParseTagChange())
                {
                    yield return true;
                    continue;
                }

                foreach (var ret in this.ParseShowEntities())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                if (HideEntityRegex.IsMatch(this.parsing_log))
                {
                    var match = HideEntityRegex.Match(this.parsing_log);
                    int entityId = Parsers.ParserUtilities.GetEntityIdFromRawString(this.game_state, match.Groups["entity"].Value);
                    this.game_state.ChangeTag(entityId, match.Groups["tag"].Value, match.Groups["value"].Value);
                    yield return true;
                    continue;
                }

                foreach (var ret in this.ParseAction())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                break;
            }
        }

        private bool ParseCreatingTag(int entity_id)
        {
            if (!CreationTagRegex.IsMatch(this.parsing_log)) return false;

            var match = CreationTagRegex.Match(this.parsing_log);
            this.game_state.ChangeTag(entity_id, match.Groups["tag"].Value, match.Groups["value"].Value);
            return true;
        }

        private bool ParseTagChange()
        {
            if (!TagChangeRegex.IsMatch(this.parsing_log)) return false;

            var match = TagChangeRegex.Match(this.parsing_log);
            var entity_raw = match.Groups["entity"].Value;
            int entityId = Parsers.ParserUtilities.GetEntityIdFromRawString(this.game_state, entity_raw);

            if (entityId >= 0)
            {
                this.game_state.ChangeTag(entityId, match.Groups["tag"].Value, match.Groups["value"].Value);
            }
            else
            {
                // failed to get entity id
                // save the information to tmp_entities; insert to game state when the entity id is parsed
                var tmpEntity = this.tmp_entities.FirstOrDefault(x => x.Name == entity_raw);
                if (tmpEntity == null)
                {
                    tmpEntity = new GameState.Entity(this.tmp_entities.Count + 1) { Name = entity_raw };
                    this.tmp_entities.Add(tmpEntity);
                }

                var tag_value = GameState.Entity.ParseTag(match.Groups["tag"].Value, match.Groups["value"].Value);
                tmpEntity.SetTag(tag_value.Item1, tag_value.Item2);
                if (tmpEntity.HasTag(GameTag.ENTITY_ID))
                {
                    var id = tmpEntity.GetTag(GameTag.ENTITY_ID);
                    if (!this.game_state.Entities.ContainsKey(id))
                    {
                        this.frm_main.AddLog("[ERROR] The temporary entity (" + entity_raw + ") now has entity_id, but it is missing from the entities set.");
                    }
                    else {
                        this.game_state.Entities[id].Name = tmpEntity.Name;
                        foreach (var t in tmpEntity.Tags)
                            this.game_state.ChangeTag(id, t.Key, t.Value);
                        this.tmp_entities.Remove(tmpEntity);
                    }
                }
            }

            return true;
        }

        // return for each consumed line
        // break if error and the line is not consumed
        private IEnumerable<bool> ParseShowEntities()
        {
            if (!ShowEntityRegex.IsMatch(this.parsing_log)) yield break;

            var match = ShowEntityRegex.Match(this.parsing_log);
            var cardId = match.Groups["cardId"].Value;
            int entityId = Parsers.ParserUtilities.GetEntityIdFromRawString(this.game_state, match.Groups["entity"].Value);

            if (entityId < 0)
            {
                this.frm_main.AddLog("[ERROR] Parse error: cannot find entity id. '" + this.parsing_log + "'");
                yield return false;
            }

            if (!this.game_state.Entities.ContainsKey(entityId))
                this.game_state.Entities.Add(entityId, new GameState.Entity(entityId));
            this.game_state.Entities[entityId].CardId = cardId;
            yield return true;

            while (true)
            {
                if (this.ParseCreatingTag(entityId)) yield return true;
                else break;
            }
        }

        private IEnumerable<bool> ParseFullEntity(string action_block_type = "")
        {
            if (!FullEntityRegex.IsMatch(this.parsing_log)) yield break;

            var match = FullEntityRegex.Match(this.parsing_log);
            var id = int.Parse(match.Groups["id"].Value);
            var cardId = match.Groups["cardId"].Value;
            if (!this.game_state.Entities.ContainsKey(id))
            {
                this.game_state.Entities.Add(id, new GameState.Entity(id) { CardId = cardId });
            }

            // triggers
            if (action_block_type == "JOUST")
            {
                this.game_state.joust_information.AddEntityId(id);
            }

            yield return true;

            while (true)
            {
                if (this.ParseCreatingTag(id)) yield return true;
                else break;
            }
        }

        private IEnumerable<bool> ParseAction()
        {
            if (!ActionStartRegex.IsMatch(this.parsing_log)) yield break;

            var match = ActionStartRegex.Match(this.parsing_log);

            var indent = match.Groups["indent"].Value;
            var entity_raw = match.Groups["entity"].Value.Trim();
            var block_type = match.Groups["block_type"].Value.Trim();
            var index = match.Groups["index"].Value.Trim();
            var target_raw = match.Groups["target"].Value.Trim();

            var entity_id = Parsers.ParserUtilities.GetEntityIdFromRawString(this.game_state, entity_raw);
            if (entity_id < 0) this.frm_main.AddLog("[INFO] Cannot get entity id for action.");

            var target_id = Parsers.ParserUtilities.GetEntityIdFromRawString(this.game_state, target_raw);

            if (block_type != "TRIGGER")
            {
                /*
                this.frm_main.AddLog("[INFO] Got action. entity = " + entity_id.ToString() +
                    " block type = " + block_type +
                    " index = " + index +
                    " target = " + target_id.ToString());
                    */
            }
            yield return true;

            while (true)
            {
                // possible logs within an action block
                bool rule_matched = false;

                if (ActionEndRegex.IsMatch(this.parsing_log))
                {
                    break;
                }

                foreach (var ret in this.ParseFullEntity(block_type))
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                foreach (var ret in this.ParseShowEntities())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                if (this.ParseTagChange())
                {
                    yield return true;
                    continue;
                }

                if (HideEntityRegex.IsMatch(this.parsing_log))
                {
                    match = HideEntityRegex.Match(this.parsing_log);
                    int entityId = Parsers.ParserUtilities.GetEntityIdFromRawString(this.game_state, match.Groups["entity"].Value);
                    this.game_state.ChangeTag(entityId, match.Groups["tag"].Value, match.Groups["value"].Value);
                    yield return true;
                    continue;
                }

                foreach (var ret in this.ParseAction())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                if (MetadataRegex.IsMatch(this.parsing_log))
                {
                    yield return true; // ignore
                    while (MetadataInfoRegex.IsMatch(this.parsing_log))
                    {
                        yield return true;
                    }
                    continue;
                }

                this.frm_main.AddLog("[ERROR] Parse failed within action (ignoring): " + this.parsing_log);
                yield return true;
            }

            yield return true;
        }
    }
}
