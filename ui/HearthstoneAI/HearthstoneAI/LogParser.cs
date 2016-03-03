using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace HearthstoneAI
{
    class LogParser
    {
        public static readonly Regex CreateGame = new Regex(@"^[\s]*CREATE_GAME");

        public static readonly Regex GameEntityRegex = new Regex(@"^[\s]*GameEntity\ EntityID=(?<id>(\d+))");

        public static readonly Regex ActionStartRegex =
            new Regex(@"^(?<indent>[\s]*)ACTION_START[\s]+Entity=(?<entity>(.*))[\s]+BlockType=(?<block_type>.*)[\s+]Index=(?<index>.*)[\s]+Target=(?<target>.*)");
        public static readonly Regex ActionEndRegex = new Regex(@"^[\s]*ACTION_END");

        public static readonly Regex CardIdRegex = new Regex(@"cardId=(?<cardId>(\w+))");

        public static readonly Regex FullEntityRegex = new Regex(@"^[\s]*FULL_ENTITY - Creating[\s]+ID=(?<id>(\d+))[\s]+CardID=(?<cardId>(\w*))");
        public static readonly Regex CreationTagRegex = new Regex(@"^[\s]*tag=(?<tag>(\w+))\ value=(?<value>(\w+))");


        public static readonly Regex PlayerEntityRegex =
            new Regex(@"^[\s]*Player\ EntityID=(?<id>(\d+))\ PlayerID=(?<playerId>(\d+))\ GameAccountId=(?<gameAccountId>(.+))");

        public static readonly Regex TagChangeRegex =
            new Regex(@"^[\s]*TAG_CHANGE\ Entity=(?<entity>(.+))\ tag=(?<tag>(\w+))\ value=(?<value>(\w+))");

        public static readonly Regex ShowEntityRegex =
            new Regex(@"^[\s]*SHOW_ENTITY\ -\ Updating\ Entity=(?<entity>(.+))\ CardID=(?<cardId>(\w*))");

        public static readonly Regex HideEntityRegex =
            new Regex(@"^[\s]*HIDE_ENTITY\ -\ Entity=(?<entity>(.+))\ tag=(?<tag>(\w+))\ value=(?<value>(\w+))");

        public static readonly Regex MetadataRegex = new Regex(@"^[\s]*META_DATA\ -\ ");
        public static readonly Regex MetadataInfoRegex = new Regex(@"^[\s]*Info\[\d+\]");

        public static readonly Regex EntityChoicesCreate =
            new Regex(@"^[\s]*id=(?<id>(.*))\ Player=(?<player_entity>.*)\ TaskList=(.*)\ ChoiceType=(?<choice_type>.*)\ CountMin=(.*)\ CountMax=(.*)");
        public static readonly Regex EntityChoicesSource =
            new Regex(@"^[\s]*Source=(?<source>.*)$");
        public static readonly Regex EntityChoicesEntities =
            new Regex(@"^[\s]*Entities\[(?<idx>.*)\]=(?<entity>(.+))$");

        public GameState GameState { get; }

        private readonly List<GameState.Entity> tmp_entities = new List<GameState.Entity>();

        public LogParser(frmMain frm)
        {
            this.frmMain = frm;
            this.GameState = new GameState();
            this.actions_nested_count = 0;
        }

        private frmMain frmMain;

        private static string PowerLogPrefix = "GameState.DebugPrintPower() - ";
        private static string EntityChoicesLogPrefix = "GameState.DebugPrintEntityChoices() - ";
        private static string SendChoicesLogPrefix = "GameState.SendChoices() - ";
        private static string PowerTaskListDebugDumpLogPrefix = "PowerTaskList.DebugDump() - ";
        private static string PowerTaskListDebugPrintPowerLogPrefix = "PowerTaskList.DebugPrintPower() - ";

        private enum ParsingStage
        {
            STAGE_OK, // basic game entity and two players' entities are parsed
            STAGE_WAITING // waiting to more log for a valid game state
        }
        private int actions_nested_count;
        private ParsingStage parsing_stage;

        private string parsing_log;
        public string[] new_log_lines; // for Process()

        public bool IsParseSuccess()
        {
            if (this.parsing_stage != ParsingStage.STAGE_OK) return false;

            // the ACTION_END is not present when doing mulligan
            // so we don't check ACTION_END
            //if (this.actions_nested_count != 0) return false;

            return true;
        }

        // return true if still parsing; false if all lines are parsed
        // never break
        public IEnumerable<bool> Process()
        {
            Parsers.SendChoicesParser send_choices_parser = new Parsers.SendChoicesParser(this.frmMain, this.GameState);

            this.parsing_stage = ParsingStage.STAGE_WAITING;

            IEnumerator<bool> power_log = ParsePowerLog().GetEnumerator();
            IEnumerator<bool> entity_choices_log = ParseEntityChoicesLog().GetEnumerator();
            IEnumerator<bool> send_choices_log = send_choices_parser.Process().GetEnumerator();

            while (true)
            {
                foreach (var log_line in this.new_log_lines)
                {
                    if (log_line == "") continue;

                    LogItem log_item;

                    try { log_item = LogItem.Parse(log_line); }
                    catch (Exception ex)
                    {
                        this.frmMain.AddLog("Failed when parsing: " + log_line);
                        continue;
                    }

                    string log = log_item.Content;

                    if (log.StartsWith(PowerLogPrefix))
                    {
                        this.parsing_log = log.Substring(PowerLogPrefix.Length);
                        power_log.MoveNext();
                        yield return true;
                    }
                    else if (log.StartsWith(EntityChoicesLogPrefix))
                    {
                        this.parsing_log = log.Substring(EntityChoicesLogPrefix.Length);
                        entity_choices_log.MoveNext();
                        yield return true;
                    }
                    else if (log.StartsWith(SendChoicesLogPrefix))
                    {
                        send_choices_parser.parsing_log = log.Substring(SendChoicesLogPrefix.Length);
                        send_choices_log.MoveNext();
                        yield return true;
                    }
                    else if (log.StartsWith(PowerTaskListDebugDumpLogPrefix)) { }
                    else if (log.StartsWith(PowerTaskListDebugPrintPowerLogPrefix)) { }
                    else
                    {
                        //this.frmMain.AddLog("Failed when parsing: " + log_line);
                    }
                }

                yield return false;
            }
        }

        private bool ParseIfMatched_CreatingTag(int entity_id)
        {
            if (!CreationTagRegex.IsMatch(this.parsing_log)) return false;

            var match = CreationTagRegex.Match(this.parsing_log);
            GameState.ChangeTag(entity_id, match.Groups["tag"].Value, match.Groups["value"].Value);
            return true;
        }

        private bool ParseIfMatched_TagChange()
        {
            if (!TagChangeRegex.IsMatch(this.parsing_log)) return false;

            var match = TagChangeRegex.Match(this.parsing_log);
            var entity_raw = match.Groups["entity"].Value;
            int entityId = Parsers.ParserUtilities.GetEntityIdFromRawString(this.GameState, entity_raw);

            if (entityId >= 0)
            {
                GameState.ChangeTag(entityId, match.Groups["tag"].Value, match.Groups["value"].Value);
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
                    if (!GameState.Entities.ContainsKey(id))
                    {
                        this.frmMain.AddLog("[ERROR] The temporary entity (" + entity_raw + ") now has entity_id, but it is missing from the entities set.");
                    }
                    else {
                        GameState.Entities[id].Name = tmpEntity.Name;
                        foreach (var t in tmpEntity.Tags)
                            GameState.ChangeTag(id, t.Key, t.Value);
                        this.tmp_entities.Remove(tmpEntity);
                    }
                }
            }

            return true;
        }

        // return for each consumed line
        // break if error and the line is not consumed
        private IEnumerable<bool> ParsePowerLog_ShowEntities(string action_block_type)
        {
            if (!ShowEntityRegex.IsMatch(this.parsing_log)) yield break;

            var match = ShowEntityRegex.Match(this.parsing_log);
            var cardId = match.Groups["cardId"].Value;
            int entityId = Parsers.ParserUtilities.GetEntityIdFromRawString(this.GameState, match.Groups["entity"].Value);

            if (entityId < 0)
            {
                this.frmMain.AddLog("[ERROR] Parse error: cannot find entity id. '" + this.parsing_log + "'");
                yield return false;
            }

            if (!GameState.Entities.ContainsKey(entityId))
                GameState.Entities.Add(entityId, new GameState.Entity(entityId));
            GameState.Entities[entityId].CardId = cardId;
            yield return true;

            while (true)
            {
                if (this.ParseIfMatched_CreatingTag(entityId)) yield return true;
                else break;
            }
        }

        private IEnumerable<bool> ParsePowerLog_FullEntity(string action_block_type = "")
        {
            if (!FullEntityRegex.IsMatch(this.parsing_log)) yield break;

            var match = FullEntityRegex.Match(this.parsing_log);
            var id = int.Parse(match.Groups["id"].Value);
            var cardId = match.Groups["cardId"].Value;
            if (!GameState.Entities.ContainsKey(id))
            {
                GameState.Entities.Add(id, new GameState.Entity(id) { CardId = cardId });
            }

            // triggers
            if (action_block_type == "JOUST")
            {
                GameState.joust_information.AddEntityId(id);
            }

            yield return true;

            while (true)
            {
                if (this.ParseIfMatched_CreatingTag(id)) yield return true;
                else break;
            }
        }

        private int GetIndent(string log)
        {
            int i = 0;
            foreach (char c in log)
            {
                if (c == ' ') i++;
                else break;
            }
            return i;
        }

        // if at least one item is returned,
        // upon exits, the last line is guarnteed to be ACTION_END, and that line is not consumed
        private IEnumerable<bool> ParsePowerLog_Action()
        {
            if (!ActionStartRegex.IsMatch(this.parsing_log)) yield break;

            this.actions_nested_count++;

            var match = ActionStartRegex.Match(this.parsing_log);

            var indent = match.Groups["indent"].Value;
            var entity_raw = match.Groups["entity"].Value.Trim();
            var block_type = match.Groups["block_type"].Value.Trim();
            var index = match.Groups["index"].Value.Trim();
            var target_raw = match.Groups["target"].Value.Trim();

            var entity_id = Parsers.ParserUtilities.GetEntityIdFromRawString(this.GameState, entity_raw);
            if (entity_id < 0) this.frmMain.AddLog("[INFO] Cannot get entity id for action.");

            var target_id = Parsers.ParserUtilities.GetEntityIdFromRawString(this.GameState, target_raw);

            if (block_type != "TRIGGER")
            {
                this.frmMain.AddLog("[INFO] Got action. entity = " + entity_id.ToString() +
                    " block type = " + block_type +
                    " index = " + index +
                    " target = " + target_id.ToString());
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

                foreach (var ret in this.ParsePowerLog_FullEntity(block_type))
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                foreach (var ret in this.ParsePowerLog_ShowEntities(block_type))
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                if (this.ParseIfMatched_TagChange())
                {
                    yield return true;
                    continue;
                }

                if (HideEntityRegex.IsMatch(this.parsing_log))
                {
                    match = HideEntityRegex.Match(this.parsing_log);
                    int entityId = Parsers.ParserUtilities.GetEntityIdFromRawString(this.GameState, match.Groups["entity"].Value);
                    GameState.ChangeTag(entityId, match.Groups["tag"].Value, match.Groups["value"].Value);
                    yield return true;
                    continue;
                }

                foreach (var ret in this.ParsePowerLog_Action())
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

                this.frmMain.AddLog("[ERROR] Parse failed within action (ignoring): " + this.parsing_log);
                yield return true;
            }

            this.actions_nested_count--;
            yield return true;
        }

        private IEnumerable<bool> ParsePowerLog_CreateGame()
        {
            if (!CreateGame.IsMatch(this.parsing_log)) yield break;
            yield return true;

            // a CREATE_GAME will present for every 30 minutes
            // or maybe when you re-connected to the game?
            // So here we don't reset the game unless the Game State is COMPLETE
            if (GameState.GameEntityId > 0)
            {
                var game_entity = GameState.Entities[GameState.GameEntityId];
                if (game_entity.GetTagOrDefault(GameTag.STATE, (int)TAG_STATE.RUNNING) == (int)TAG_STATE.COMPLETE)
                {
                    this.GameState.Reset();
                }
            }
            //GameState.Reset();

            if (!GameEntityRegex.IsMatch(this.parsing_log))
            {
                this.frmMain.AddLog("[ERROR] Expecting the game entity log.");
                yield break;
            }

            var game_entity_match = GameEntityRegex.Match(this.parsing_log);
            var game_entity_id = int.Parse(game_entity_match.Groups["id"].Value);
            GameState.CreateGameEntity(game_entity_id);
            yield return true;

            while (true)
            {
                if (this.ParseIfMatched_CreatingTag(game_entity_id)) yield return true;
                else break;
            }

            for (int i = 0; i < 2; i++) // two players
            {
                if (!PlayerEntityRegex.IsMatch(this.parsing_log))
                {
                    this.frmMain.AddLog("[ERROR] Expecting the player entity log.");
                    yield break;
                }

                var match = PlayerEntityRegex.Match(this.parsing_log);
                var id = int.Parse(match.Groups["id"].Value);
                if (!GameState.Entities.ContainsKey(id))
                    GameState.Entities.Add(id, new GameState.Entity(id));
                if (i == 1) this.parsing_stage = ParsingStage.STAGE_OK;
                yield return true;

                while (true)
                {
                    if (this.ParseIfMatched_CreatingTag(id)) yield return true;
                    else break;
                }
            }

            while (true)
            {
                bool rule_matched = false;

                foreach (var ret in this.ParsePowerLog_FullEntity())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                if (this.ParseIfMatched_TagChange())
                {
                    yield return true;
                    continue;
                }

                foreach (var ret in this.ParsePowerLog_Action())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                break;
            }
        }

        private IEnumerable<bool> ParsePowerLog()
        {
            while (true)
            {
                bool rule_matched = false;

                foreach (var ret in this.ParsePowerLog_CreateGame())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                this.frmMain.AddLog("[ERROR] Parse power log failed: " + this.parsing_log);
                yield return true;
            }
        }

        private IEnumerable<bool> ParseIfMatch_EntityChoiceCreate()
        {
            if (!EntityChoicesCreate.IsMatch(this.parsing_log)) yield break;

            var match = EntityChoicesCreate.Match(this.parsing_log);

            int entity_choice_id = -1;
            if (int.TryParse(match.Groups["id"].Value.Trim(), out entity_choice_id) == false)
            {
                this.frmMain.AddLog("[INFO] Cannot get entity choice id (ignoring): " + this.parsing_log);
            }

            var player_entity_raw = match.Groups["player_entity"].Value.Trim();
            int player_entity_id = Parsers.ParserUtilities.GetEntityIdFromRawString(this.GameState, player_entity_raw);

            var choice_type = match.Groups["choice_type"].Value.Trim();

            if (GameState.EntityChoices.ContainsKey(entity_choice_id))
            {
                this.frmMain.AddLog("[ERROR] Entity choice index overlapped (overwritting)");
                GameState.EntityChoices.Remove(entity_choice_id);
            }

            GameState.EntityChoices[entity_choice_id] = new GameState.EntityChoice();
            GameState.EntityChoices[entity_choice_id].id = entity_choice_id;
            GameState.EntityChoices[entity_choice_id].choice_type = choice_type;
            GameState.EntityChoices[entity_choice_id].player_entity_id = player_entity_id;
            GameState.EntityChoices[entity_choice_id].choices_has_sent = false;

            yield return true;

            while (true)
            {
                if (!ParseIfMatch_EntityChoiceSource(entity_choice_id))
                {
                    this.frmMain.AddLog("[INFO] Waiting for entity source, but got " + this.parsing_log + " (ignoring)");
                    continue;
                }
                yield return true;
                break;
            }

            while (true)
            {
                if (ParseIfMatch_EntityChoiceEntity(entity_choice_id)) yield return true;
                else break;
            }
        }

        private bool ParseIfMatch_EntityChoiceSource(int entity_choice_id)
        {
            if (!EntityChoicesSource.IsMatch(this.parsing_log)) return false;

            var match = EntityChoicesSource.Match(this.parsing_log);
            GameState.EntityChoices[entity_choice_id].source = match.Groups["source"].Value.Trim();

            return true;
        }

        private bool ParseIfMatch_EntityChoiceEntity(int entity_choice_id)
        {
            if (!EntityChoicesEntities.IsMatch(this.parsing_log)) return false;

            var match = EntityChoicesEntities.Match(this.parsing_log);

            if (GameState.EntityChoices.ContainsKey(entity_choice_id) == false)
            {
                this.frmMain.AddLog("[ERROR] missing current entity choice id");
                return false;
            }

            int idx;
            if (int.TryParse(match.Groups["idx"].Value.Trim(), out idx) == false)
            {
                this.frmMain.AddLog("[ERROR] Parse failed: " + this.parsing_log);
                return false;
            }

            var entity_raw = match.Groups["entity"].Value.Trim();
            int entity_id = Parsers.ParserUtilities.GetEntityIdFromRawString(this.GameState, entity_raw);
            if (entity_id < 0)
            {
                this.frmMain.AddLog("[ERROR] Failed to get entity id: " + this.parsing_log);
                return false;
            }

            GameState.EntityChoices[entity_choice_id].choices[idx] = entity_id;

            // Trigger
            this.EntityChoiceAdded(entity_choice_id);
            return true;
        }

        private IEnumerable<bool> ParseEntityChoicesLog()
        {
            while (true)
            {
                bool rule_matched = false;

                foreach (var ret in this.ParseIfMatch_EntityChoiceCreate())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                this.frmMain.AddLog("[ERROR] Unhandled entity chocies log: " + this.parsing_log);
                yield return true;
            }
        }

        private void EntityChoiceAdded(int entity_choice_id)
        {
            var choice = GameState.EntityChoices[entity_choice_id];
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

            if (GameState.PlayerEntityId < 0 && GameState.Entities[choice].CardId != "")
            {
                GameState.PlayerEntityId = mulligan.player_entity_id;
            }
            
            if (GameState.OpponentEntityId < 0 && GameState.Entities[choice].CardId == "")
            {
                GameState.OpponentEntityId = mulligan.player_entity_id;
            }
        }
    }
}
