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
            new Regex(@"^[\s]*ACTION_START[\s]+Entity=(?<entity>(.*))[\s]+BlockType=(?<block_type>.*)[\s+]Index=(?<index>.*)[\s]+Target=(?<target>.*)");
        public static readonly Regex ActionEndRegex = new Regex(@"^[\s]*ACTION_END");

        public static readonly Regex CardIdRegex = new Regex(@"cardId=(?<cardId>(\w+))");

        public static readonly Regex FullEntityRegex = new Regex(@"^[\s]*FULL_ENTITY - Creating[\s]+ID=(?<id>(\d+))[\s]+CardID=(?<cardId>(\w*))");
        public static readonly Regex CreationTagRegex = new Regex(@"^[\s]*tag=(?<tag>(\w+))\ value=(?<value>(\w+))");

        public static readonly Regex EntityRegex =
            new Regex(
                @"(?=id=(?<id>(\d+)))(?=name=(?<name>(\w+)))?(?=zone=(?<zone>(\w+)))?(?=zonePos=(?<zonePos>(\d+)))?(?=cardId=(?<cardId>(\w+)))?(?=player=(?<player>(\d+)))?(?=type=(?<type>(\w+)))?");


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
        }

        private frmMain frmMain;

        private static string PowerLogPrefix = "GameState.DebugPrintPower() - ";
        private static string EntityChoicesLogPrefix = "GameState.DebugPrintEntityChoices() - ";
        private static string PowerTaskListDebugDumpLogPrefix = "PowerTaskList.DebugDump() - ";
        private static string PowerTaskListDebugPrintPowerLogPrefix = "PowerTaskList.DebugPrintPower() - ";

        private string parsing_log;
        public IEnumerable<bool> Process(string [] log_lines)
        {
            IEnumerator<bool> power_log = ParsePowerLog().GetEnumerator();
            IEnumerator<bool> entity_choices_log = ParseEntityChoicesLog().GetEnumerator();

            foreach (var log_line in log_lines)
            {
                if (log_line == "") continue;

                LogItem log_item;

                try { log_item = LogItem.Parse(log_line); }
                catch (Exception ex) {
                    this.frmMain.AddLog("Failed when parsing: " + log_line);
                    continue;
                }

                string log = log_item.Content;

                if (log.StartsWith(PowerLogPrefix))
                {
                    this.parsing_log = log.Substring(PowerLogPrefix.Length);
                    power_log.MoveNext();
                    yield return power_log.Current;
                }
                else if (log.StartsWith(EntityChoicesLogPrefix))
                {
                    this.parsing_log = log.Substring(EntityChoicesLogPrefix.Length);
                    entity_choices_log.MoveNext();
                    yield return entity_choices_log.Current;
                }
                else if (log.StartsWith(PowerTaskListDebugDumpLogPrefix)) { }
                else if (log.StartsWith(PowerTaskListDebugPrintPowerLogPrefix)) { }
                else
                {
                    //this.frmMain.AddLog("Failed when parsing: " + log_line);
                }
            }
        }

        private int GetEntityIdFromRawString(string rawEntity)
        {
            rawEntity = rawEntity.Replace("UNKNOWN ENTITY ", "");
            int entityId = -1;

            // Get entity id - Method 1
            if (rawEntity.StartsWith("[") && EntityRegex.IsMatch(rawEntity))
            {
                entityId = int.Parse(EntityRegex.Match(rawEntity).Groups["id"].Value);
            }

            // Get entity id - Method 2
            if (entityId < 0)
            {
                if (!int.TryParse(rawEntity, out entityId)) entityId = -1;
            }

            // Get entity id - Method 3
            if (entityId < 0)
            {
                var entity = GameState.Entities.FirstOrDefault(x => x.Value.Name == rawEntity);

                if (entity.Value == null)
                {
                    // TODO: check when this happens?
                    entity = GameState.Entities.FirstOrDefault(x => x.Value.Name == "UNKNOWN HUMAN PLAYER");
                    if (entity.Value != null) entity.Value.Name = rawEntity;
                }

                if (entity.Value != null) entityId = entity.Key;
                else entityId = -1;
            }

            return entityId;
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
            int entityId = GetEntityIdFromRawString(entity_raw);

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
                tmpEntity.SetTag(tag_value);
                if (tmpEntity.HasTag(GameTag.ENTITY_ID))
                {
                    var id = tmpEntity.GetTag(GameTag.ENTITY_ID);
                    if (!GameState.Entities.ContainsKey(id))
                    {
                        this.frmMain.AddLog("[ERROR] TMP ENTITY (" + entity_raw + ") NOW HAS A KEY, BUT GAME.ENTITIES DOES NOT CONTAIN THIS KEY");
                    }
                    else {
                        GameState.Entities[id].Name = tmpEntity.Name;
                        foreach (var t in tmpEntity.Tags)
                            GameState.Entities[id].SetTag(t.Key, t.Value);
                        this.tmp_entities.Remove(tmpEntity);
                    }
                }
            }

            return true;
        }

        // return for each consumed line
        // break if error and the line is not consumed
        private IEnumerable<bool> ParsePowerLog_ShowEntities()
        {
            if (!ShowEntityRegex.IsMatch(this.parsing_log)) yield break;

            var match = ShowEntityRegex.Match(this.parsing_log);
            var cardId = match.Groups["cardId"].Value;
            int entityId = GetEntityIdFromRawString(match.Groups["entity"].Value);

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

        private IEnumerable<bool> ParsePowerLog_FullEntity()
        {
            if (!FullEntityRegex.IsMatch(this.parsing_log)) yield break;
            var match = FullEntityRegex.Match(this.parsing_log);
            var id = int.Parse(match.Groups["id"].Value);
            var cardId = match.Groups["cardId"].Value;
            if (!GameState.Entities.ContainsKey(id))
            {
                GameState.Entities.Add(id, new GameState.Entity(id) { CardId = cardId });
            }
            yield return true;

            while (true)
            {
                if (this.ParseIfMatched_CreatingTag(id)) yield return true;
                else break;
            }
        }

        private IEnumerable<bool> ParsePowerLog()
        {
            while (true)
            {
                bool rule_matched = false;

                //this.frmMain.AddLog("[Read Power Log] " + this.parsing_log);

                if (CreateGame.IsMatch(this.parsing_log))
                {
                    yield return true;
                    continue;
                }

                if (GameEntityRegex.IsMatch(this.parsing_log))
                {
                    var match = GameEntityRegex.Match(this.parsing_log);
                    var id = int.Parse(match.Groups["id"].Value);
                    GameState.CreateGameEntity(id);
                    yield return true;

                    while (true)
                    {
                        if (this.ParseIfMatched_CreatingTag(id)) yield return true;
                        else break;
                    }
                    continue;
                }

                if (PlayerEntityRegex.IsMatch(this.parsing_log))
                {
                    var match = PlayerEntityRegex.Match(this.parsing_log);
                    var id = int.Parse(match.Groups["id"].Value);
                    if (!GameState.Entities.ContainsKey(id))
                        GameState.Entities.Add(id, new GameState.Entity(id));
                    yield return true;

                    while (true)
                    {
                        if (this.ParseIfMatched_CreatingTag(id)) yield return true;
                        else break;
                    }
                    continue;
                }

                foreach (var ret in this.ParsePowerLog_FullEntity())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                foreach (var ret in this.ParsePowerLog_ShowEntities())
                {
                    rule_matched = true;
                    yield return ret;
                }
                if (rule_matched) continue;

                if (this.ParseIfMatched_TagChange()) continue;

                if (HideEntityRegex.IsMatch(this.parsing_log))
                {
                    var match = HideEntityRegex.Match(this.parsing_log);
                    int entityId = GetEntityIdFromRawString(match.Groups["entity"].Value);
                    GameState.ChangeTag(entityId, match.Groups["tag"].Value, match.Groups["value"].Value);
                    yield return true;
                    continue;
                }

                if (ActionStartRegex.IsMatch(this.parsing_log))
                {
                    var match = ActionStartRegex.Match(this.parsing_log);

                    var entity_raw = match.Groups["entity"].Value.Trim();
                    var block_type = match.Groups["block_type"].Value.Trim();
                    var index = match.Groups["index"].Value.Trim();
                    var target_raw = match.Groups["target"].Value.Trim();

                    var entity_id = this.GetEntityIdFromRawString(entity_raw);
                    if (entity_id < 0)
                    {
                        this.frmMain.AddLog("[ERROR] Cannot get entity id for action.");
                        yield return false;
                    }

                    var target_id = this.GetEntityIdFromRawString(target_raw);

                    if (block_type != "TRIGGER")
                    {
                        this.frmMain.AddLog("[INFO] Got action. entity = " + entity_id.ToString() +
                            " block type = " + block_type +
                            " index = " + index +
                            " target = " + target_id.ToString());
                    }
                    yield return true;
                    continue;
                }

                if (ActionEndRegex.IsMatch(this.parsing_log))
                {
                    yield return true;
                    continue;
                }

                if (MetadataRegex.IsMatch(this.parsing_log))
                {
                    yield return true; // ignore
                    while (MetadataInfoRegex.IsMatch(this.parsing_log))
                    {
                        yield return true;
                    }
                    continue;
                }

                this.frmMain.AddLog("[ERROR] Parse power log failed: " + this.parsing_log);
                yield return true;
            }
        }

        private bool ParseIfMatch_EntityChoiceCreate(out int entity_choice_id)
        {
            entity_choice_id = -1;
            if (!EntityChoicesCreate.IsMatch(this.parsing_log)) return false;

            var match = EntityChoicesCreate.Match(this.parsing_log);

            if (int.TryParse(match.Groups["id"].Value.Trim(), out entity_choice_id) == false)
            {
                this.frmMain.AddLog("[ERROR] Parse failed: " + this.parsing_log);
                return false;
            }

            var player_entity_raw = match.Groups["player_entity"].Value.Trim();
            int player_entity_id = GetEntityIdFromRawString(player_entity_raw);

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

            return true;
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
            int entity_id = GetEntityIdFromRawString(entity_raw);
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
                int entity_choice_id;

                if (ParseIfMatch_EntityChoiceCreate(out entity_choice_id))
                {
                    yield return true;

                    if (ParseIfMatch_EntityChoiceSource(entity_choice_id)) yield return true;
                    else continue;

                    while (true)
                    {
                        if (ParseIfMatch_EntityChoiceEntity(entity_choice_id)) yield return true;
                        else break;
                    }
                    continue;
                }
                else
                {
                    this.frmMain.AddLog("[ERROR] Unhandled entity chocies log: " + this.parsing_log);
                    yield return true;
                }

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
