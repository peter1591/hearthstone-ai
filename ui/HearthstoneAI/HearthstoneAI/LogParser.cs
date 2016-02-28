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

        public static readonly Regex CreationRegex = new Regex(@"^[\s]*FULL_ENTITY - Creating[\s]+ID=(?<id>(\d+))[\s]+CardID=(?<cardId>(\w*))");
        public static readonly Regex CreationTagRegex = new Regex(@"^[\s]*tag=(?<tag>(\w+))\ value=(?<value>(\w+))");

        public static readonly Regex EntityRegex =
            new Regex(
                @"(?=id=(?<id>(\d+)))(?=name=(?<name>(\w+)))?(?=zone=(?<zone>(\w+)))?(?=zonePos=(?<zonePos>(\d+)))?(?=cardId=(?<cardId>(\w+)))?(?=player=(?<player>(\d+)))?(?=type=(?<type>(\w+)))?");


        public static readonly Regex PlayerEntityRegex =
            new Regex(@"^[\s]*Player\ EntityID=(?<id>(\d+))\ PlayerID=(?<playerId>(\d+))\ GameAccountId=(?<gameAccountId>(.+))");

        public static readonly Regex TagChangeRegex =
            new Regex(@"^[\s]*TAG_CHANGE\ Entity=(?<entity>(.+))\ tag=(?<tag>(\w+))\ value=(?<value>(\w+))");

        public static readonly Regex UpdatingEntityRegex =
            new Regex(@"^[\s]*SHOW_ENTITY\ -\ Updating\ Entity=(?<entity>(.+))\ CardID=(?<cardId>(\w*))");

        public static readonly Regex HideEntityRegex =
            new Regex(@"^[\s]*HIDE_ENTITY\ -\ Entity=(?<entity>(.+))\ tag=(?<tag>(\w+))\ value=(?<value>(\w+))");


        public static readonly Regex EntityChoicesCreate =
            new Regex(@"^[\s]*id=(?<id>(.*))\ Player=(?<player_entity>.*)\ TaskList=(.*)\ ChoiceType=(?<choice_type>.*)\ CountMin=(.*)\ CountMax=(.*)");
        public static readonly Regex EntityChoicesSource =
            new Regex(@"^[\s]*Source=(?<source>.*)$");
        public static readonly Regex EntityChoicesEntities =
            new Regex(@"^[\s]*Entities\[(?<idx>.*)\]=(?<entity>(.+))$");

        public GameState GameState { get; }

        private int current_entity_id;
        private readonly List<GameState.Entity> tmp_entities = new List<GameState.Entity>();

        private int current_entity_choice_id;

        public LogParser(frmMain frm)
        {
            this.frmMain = frm;
            this.GameState = new GameState();
        }

        private frmMain frmMain;

        private static string PowerLogPrefix = "GameState.DebugPrintPower() - ";
        private static string EntityChoicesLogPrefix = "GameState.DebugPrintEntityChoices() - ";

        public void Process(LogItem item)
        {
            string log = item.Content;

            if (log.StartsWith(PowerLogPrefix))
            {
                ParsePowerLog(log.Substring(PowerLogPrefix.Length));
            }
            else if (log.StartsWith(EntityChoicesLogPrefix))
            {
                ParseEntityChoicesLog(log.Substring(EntityChoicesLogPrefix.Length));
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

        private void ParsePowerLog(string content)
        {
            //this.frmMain.AddLog("[Read Power Log] " + content);

            if (content.Contains("SHOW_ENTITY - Updating Entity=[id=13 cardId= type=INVALID zone=DECK zonePos=0 player=1] CardID=GVG_038"))
            {
                ;
            }

            if (GameEntityRegex.IsMatch(content))
            {
                var match = GameEntityRegex.Match(content);
                var id = int.Parse(match.Groups["id"].Value);
                GameState.CreateGameEntity(id);
                this.current_entity_id = id;
            }
            else if (PlayerEntityRegex.IsMatch(content))
            {
                var match = PlayerEntityRegex.Match(content);
                var id = int.Parse(match.Groups["id"].Value);
                if (!GameState.Entities.ContainsKey(id))
                    GameState.Entities.Add(id, new GameState.Entity(id));
                this.current_entity_id = id;
            }
            else if (TagChangeRegex.IsMatch(content))
            {
                var match = TagChangeRegex.Match(content);
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
            }
            else if (CreationRegex.IsMatch(content))
            {
                var match = CreationRegex.Match(content);
                var id = int.Parse(match.Groups["id"].Value);
                var cardId = match.Groups["cardId"].Value;
                if (!GameState.Entities.ContainsKey(id))
                {
                    GameState.Entities.Add(id, new GameState.Entity(id) { CardId = cardId });
                }
                this.current_entity_id = id;
            }
            else if (UpdatingEntityRegex.IsMatch(content))
            {
                var match = UpdatingEntityRegex.Match(content);
                var cardId = match.Groups["cardId"].Value;
                int entityId = GetEntityIdFromRawString(match.Groups["entity"].Value);

                if (entityId >= 0)
                {
                    this.current_entity_id = entityId;
                    if (!GameState.Entities.ContainsKey(entityId))
                        GameState.Entities.Add(entityId, new GameState.Entity(entityId));
                    GameState.Entities[entityId].CardId = cardId;
                }
            }
            else if (HideEntityRegex.IsMatch(content))
            {
                var match = HideEntityRegex.Match(content);
                int entityId = GetEntityIdFromRawString(match.Groups["entity"].Value);
                GameState.ChangeTag(entityId, match.Groups["tag"].Value, match.Groups["value"].Value);
            }
            else if (CreationTagRegex.IsMatch(content))
            {
                // We can skip 'HIDE_ENTITY'
                var match = CreationTagRegex.Match(content);
                GameState.ChangeTag(this.current_entity_id, match.Groups["tag"].Value, match.Groups["value"].Value);
            }
            else if (ActionStartRegex.IsMatch(content))
            {
                var match = ActionStartRegex.Match(content);

                var entity_raw = match.Groups["entity"].Value.Trim();
                var block_type = match.Groups["block_type"].Value.Trim();
                var index = match.Groups["index"].Value.Trim();
                var target_raw = match.Groups["target"].Value.Trim();

                var entity_id = this.GetEntityIdFromRawString(entity_raw);
                if (entity_id < 0)
                {
                    this.frmMain.AddLog("[ERROR] Cannot get entity id for action.");
                    return;
                }

                var target_id = this.GetEntityIdFromRawString(target_raw);

                if (block_type != "TRIGGER")
                {
                    this.frmMain.AddLog("[INFO] Got action. entity = " + entity_id.ToString() +
                        " block type = " + block_type +
                        " index = " + index +
                        " target = " + target_id.ToString());
                }
            }
            else if (CreateGame.IsMatch(content)) { }
            else if (ActionEndRegex.IsMatch(content)) { }
            else
            {
                this.frmMain.AddLog("[ERROR] Parse power log failed: " + content);
            }
        }

        private void ParseEntityChoicesLog(string content)
        {
            if (EntityChoicesCreate.IsMatch(content))
            {
                var match = EntityChoicesCreate.Match(content);

                int id;
                if (int.TryParse(match.Groups["id"].Value.Trim(), out id) == false)
                {
                    this.frmMain.AddLog("[ERROR] Parse failed: " + content);
                    return;
                }

                var player_entity_raw = match.Groups["player_entity"].Value.Trim();
                int player_entity_id = GetEntityIdFromRawString(player_entity_raw);

                var choice_type = match.Groups["choice_type"].Value.Trim();

                if (GameState.EntityChoices.ContainsKey(id))
                {
                    this.frmMain.AddLog("[ERROR] Entity choice index overlapped (overwritting)");
                    GameState.EntityChoices.Remove(id);
                }

                GameState.EntityChoices[id] = new GameState.EntityChoice();
                GameState.EntityChoices[id].id = id;
                GameState.EntityChoices[id].choice_type = choice_type;
                GameState.EntityChoices[id].player_entity_id = player_entity_id;

                this.current_entity_choice_id = id;
            }
            else if (EntityChoicesSource.IsMatch(content))
            {
                var match = EntityChoicesSource.Match(content);

                if (GameState.EntityChoices.ContainsKey(this.current_entity_choice_id) == false)
                {
                    this.frmMain.AddLog("[ERROR] missing current entity choice id");
                    return;
                }

                GameState.EntityChoices[this.current_entity_choice_id].source = match.Groups["source"].Value.Trim();
            }
            else if (EntityChoicesEntities.IsMatch(content))
            {
                var match = EntityChoicesEntities.Match(content);

                if (GameState.EntityChoices.ContainsKey(this.current_entity_choice_id) == false)
                {
                    this.frmMain.AddLog("[ERROR] missing current entity choice id");
                    return;
                }

                int idx;
                if (int.TryParse(match.Groups["idx"].Value.Trim(), out idx) == false)
                {
                    this.frmMain.AddLog("[ERROR] Parse failed: " + content);
                    return;
                }

                var entity_raw = match.Groups["entity"].Value.Trim();
                int entity_id = GetEntityIdFromRawString(entity_raw);
                if (entity_id < 0)
                {
                    this.frmMain.AddLog("[ERROR] Failed to get entity id: " + content);
                    return;
                }

                GameState.EntityChoices[this.current_entity_choice_id].choices[idx] = entity_id;

                // Trigger
                this.EntityChoiceAdded(this.current_entity_choice_id);
            }
            else
            {
                this.frmMain.AddLog("[ERROR] Unhandled entity chocies log");
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
