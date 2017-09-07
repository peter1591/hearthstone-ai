using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace HearthstoneAI.LogWatcher.SubParsers
{
    class PowerLogParser
    {
        public delegate void LogMsgDelegate(String msg);
        public LogMsgDelegate log_msg;

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

        private static readonly Regex BlockStartRegex =
            new Regex(@"^(?<indent>[\s]*)BLOCK_START[\s]+BlockType=(?<block_type>.*)[\s+]Entity=(?<entity>(.*))[\s]+EffectCardId=(?<effect_card_id>.*)[\s]+EffectIndex=(?<effect_index>.*)[\s]+Target=(?<target>.*)");
        private static readonly Regex BlockEndRegex = new Regex(@"^[\s]*BLOCK_END");

        private static readonly Regex MetadataRegex = new Regex(@"^[\s]*META_DATA\ -\ ");
        private static readonly Regex MetadataInfoRegex = new Regex(@"^[\s]*Info\[\d+\]");

        private List<GameState.Entity> tmp_entities = new List<GameState.Entity>();

        public class ActionStartEventArgs : EventArgs
        {
            public ActionStartEventArgs(ActionStartEventArgs e)
            {
                this.entity_id = e.entity_id;
                this.block_type = e.block_type;
            }

            public ActionStartEventArgs(int entity_id, string block_type)
            {
                this.entity_id = entity_id;
                this.block_type = block_type;
            }

            public int entity_id;
            public string block_type;
        }
        public event EventHandler<ActionStartEventArgs> ActionStart;

        public class CreateGameEventArgs : EventArgs { }
        public event EventHandler<CreateGameEventArgs> CreateGameEvent;

        public PowerLogParser(GameState game_state)
        {
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

                log_msg("[ERROR] Parse power log failed: " + this.parsing_log);
                yield return true;
            }
        }

        private IEnumerable<bool> ParseCreateGame()
        {
            if (!CreateGameRegex.IsMatch(this.parsing_log)) yield break;

            if (this.CreateGameEvent != null) this.CreateGameEvent(this, new CreateGameEventArgs());

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
                log_msg("[ERROR] Expecting the game entity log.");
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
                    log_msg("[ERROR] Expecting the player entity log.");
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
                bool matched = false;
                foreach (var ret in ParserUtilities.TrySubParsers(new List<Func<IEnumerable<bool>>>{
                    () => { return this.ParseFullEntity(); },
                    () => { return this.ParseTagChange(); },
                    () => { return this.ParseShowEntities(); },
                    () => { return this.ParseHideEntity(); },
                    () => { return this.ParseBlock(); }
                }))
                {
                    matched = true;
                    yield return ret;
                }

                if (!matched) yield break;
            }
        }

        private bool ParseCreatingTag(int entity_id)
        {
            if (!CreationTagRegex.IsMatch(this.parsing_log)) return false;

            var match = CreationTagRegex.Match(this.parsing_log);
            this.game_state.ChangeTag(entity_id, match.Groups["tag"].Value, match.Groups["value"].Value);
            return true;
        }

        private IEnumerable<bool> ParseTagChange()
        {
            if (!TagChangeRegex.IsMatch(this.parsing_log)) yield break;

            var match = TagChangeRegex.Match(this.parsing_log);
            var entity_raw = match.Groups["entity"].Value;
            int entityId = ParserUtilities.GetEntityIdFromRawString(this.game_state, entity_raw);

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
                        log_msg("[ERROR] The temporary entity (" + entity_raw + ") now has entity_id, but it is missing from the entities set.");
                    }
                    else {
                        this.game_state.Entities[id].Name = tmpEntity.Name;
                        foreach (var t in tmpEntity.Tags)
                            this.game_state.ChangeTag(id, t.Key, t.Value);
                        this.tmp_entities.Remove(tmpEntity);
                    }
                }
            }

            yield return true;
            yield break;
        }

        // return for each consumed line
        // break if error and the line is not consumed
        private IEnumerable<bool> ParseShowEntities()
        {
            var match = ShowEntityRegex.Match(this.parsing_log);
            if (!match.Success) yield break;

            var cardId = match.Groups["cardId"].Value;
            int entityId = ParserUtilities.GetEntityIdFromRawString(this.game_state, match.Groups["entity"].Value);

            if (entityId < 0)
            {
                log_msg("[ERROR] Parse error: cannot find entity id. '" + this.parsing_log + "'");
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

        private IEnumerable<bool> ParseHideEntity()
        {
            if (HideEntityRegex.IsMatch(this.parsing_log))
            {
                var match = HideEntityRegex.Match(this.parsing_log);
                int entityId = ParserUtilities.GetEntityIdFromRawString(this.game_state, match.Groups["entity"].Value);
                this.game_state.ChangeTag(entityId, match.Groups["tag"].Value, match.Groups["value"].Value);
                yield return true;
            }
        }

        private IEnumerable<bool> ParseMetadata()
        {
            if (MetadataRegex.IsMatch(this.parsing_log))
            {
                yield return true; // ignore

                while (MetadataInfoRegex.IsMatch(this.parsing_log))
                {
                    yield return true;
                }
            }
        }

        private IEnumerable<bool> ParseBlock()
        {
            var match = BlockStartRegex.Match(this.parsing_log);
            if (!match.Success) yield break;

            var indent = match.Groups["indent"].Value;
            var entity_raw = match.Groups["entity"].Value.Trim();
            var block_type = match.Groups["block_type"].Value.Trim();
            var target_raw = match.Groups["target"].Value.Trim();

            var entity_id = ParserUtilities.GetEntityIdFromRawString(this.game_state, entity_raw);
            if (entity_id < 0) log_msg(String.Format("[INFO] Cannot get entity id '{0}'.", entity_raw));

            var target_id = ParserUtilities.GetEntityIdFromRawString(this.game_state, target_raw);

            if (this.ActionStart != null) this.ActionStart(this, new ActionStartEventArgs(entity_id, block_type));

            yield return true;

            while (true)
            {
                if (BlockEndRegex.IsMatch(this.parsing_log))
                {
                    break;
                }

                bool matched = false;
                foreach (var ret in ParserUtilities.TrySubParsers(new List<Func<IEnumerable<bool>>>
                {
                    () => this.ParseFullEntity(),
                    () => this.ParseShowEntities(),
                    () => this.ParseTagChange(),
                    () => this.ParseHideEntity(),
                    () => this.ParseBlock(),
                    () => this.ParseMetadata()
                }))
                {
                    matched = true;
                    yield return ret;
                }

                if (!matched)
                {
                    log_msg("[ERROR] Parse failed within action (ignoring): " + this.parsing_log);
                    yield return true;
                }
            }

            if (block_type == "PLAY")
            {
                log_msg("[INFO] Got a play action. entity = " + entity_id.ToString() +
                    " eneity card id = " + this.game_state.Entities[entity_id].CardId.ToString() +
                    " block type = " + block_type +
                    " target = " + target_id.ToString());
                this.AnalyzePlayHandCardAction(entity_id);
            }

            yield return true;
        }

        private void AnalyzePlayHandCardAction(int entity_id)
        {
            // get current player
            int playing_entity = this.GetPlayingPlayerEntityID();
            if (playing_entity < 0) return;

            string playing_card = this.game_state.Entities[entity_id].CardId.ToString();

            if (playing_entity == this.game_state.PlayerEntityId)
            {
                this.game_state.player_played_hand_cards.Add(playing_card);
            }
            else if (playing_entity == this.game_state.OpponentEntityId)
            {
                this.game_state.opponent_played_hand_cards.Add(playing_card);
            }
            else return;
        }

        private int GetPlayingPlayerEntityID()
        {
            GameState.Entity game_entity;
            if (!this.game_state.TryGetGameEntity(out game_entity)) return -1;

            GameState.Entity player_entity;
            if (!this.game_state.TryGetPlayerEntity(out player_entity)) return -1;

            GameState.Entity opponent_entity;
            if (!this.game_state.TryGetOpponentEntity(out opponent_entity)) return -1;

            if (player_entity.GetTagOrDefault(GameTag.MULLIGAN_STATE, (int)TAG_MULLIGAN.INVALID) == (int)TAG_MULLIGAN.INPUT)
            {
                return -1;
            }

            if (opponent_entity.GetTagOrDefault(GameTag.MULLIGAN_STATE, (int)TAG_MULLIGAN.INVALID) == (int)TAG_MULLIGAN.INPUT)
            {
                return -1;
            }

            if (!game_entity.HasTag(GameTag.STEP)) return -1;

            TAG_STEP game_entity_step = (TAG_STEP)game_entity.GetTag(GameTag.STEP);
            if (game_entity_step != TAG_STEP.MAIN_ACTION) return -1;

            bool player_first = false;
            if (player_entity.GetTagOrDefault(GameTag.FIRST_PLAYER, 0) == 1) player_first = true;
            else if (opponent_entity.GetTagOrDefault(GameTag.FIRST_PLAYER, 0) == 1) player_first = false;
            else throw new Exception("parse failed");

            int turn = game_entity.GetTagOrDefault(GameTag.TURN, -1);
            if (turn < 0) return -1;

            if (player_first && (turn % 2 == 1)) return player_entity.Id;
            else if (!player_first && (turn % 2 == 0)) return player_entity.Id;
            else return opponent_entity.Id;
        }
    }
}
