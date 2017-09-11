using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace HearthstoneAI.LogWatcher.SubParsers
{
    class PowerLogParser : BaseParser
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

        private static readonly Regex BlockStartRegex =
            new Regex(@"^(?<indent>[\s]*)BLOCK_START[\s]+BlockType=(?<block_type>.*)[\s+]Entity=(?<entity>(.*))[\s]+EffectCardId=(?<effect_card_id>.*)[\s]+EffectIndex=(?<effect_index>.*)[\s]+Target=(?<target>[\w]*)");
        private static readonly Regex BlockEndRegex = new Regex(@"^[\s]*BLOCK_END");

        private static readonly Regex MetadataRegex = new Regex(@"^[\s]*META_DATA\ -\ ");
        private static readonly Regex MetadataInfoRegex = new Regex(@"^[\s]*Info\[\d+\]");

        public class BlockStartEventArgs : EventArgs
        {
            public BlockStartEventArgs(BlockStartEventArgs e)
            {
                this.entity_id = e.entity_id;
                this.block_type = e.block_type;
            }

            public BlockStartEventArgs(int entity_id, string block_type)
            {
                this.entity_id = entity_id;
                this.block_type = block_type;
            }

            public int entity_id;
            public string block_type;
        }
        public event EventHandler<BlockStartEventArgs> BlockStart;

        public class BlockEndEventArgs : EventArgs
        {
            public BlockEndEventArgs(BlockEndEventArgs e)
            {
                this.entity_id = e.entity_id;
                this.block_type = e.block_type;
            }

            public BlockEndEventArgs(int entity_id, string block_type)
            {
                this.entity_id = entity_id;
                this.block_type = block_type;
            }

            public int entity_id;
            public string block_type;
        }
        public event EventHandler<BlockEndEventArgs> BlockEnd;

        public class CreateGameEventArgs : EventArgs { }
        public event EventHandler<CreateGameEventArgs> CreateGameEvent;

        public PowerLogParser(Game game_state, Logger logger)
            : base(logger)
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
        private Game game_state;
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

                logger_.Info("[ERROR] Parse power log failed: " + this.parsing_log);
                yield return true;
            }
        }

        private IEnumerable<bool> ParseCreateGame()
        {
            if (!CreateGameRegex.IsMatch(this.parsing_log)) yield break;

            if (this.CreateGameEvent != null) this.CreateGameEvent(this, new CreateGameEventArgs());

            yield return true;

            if (!GameEntityRegex.IsMatch(this.parsing_log))
            {
                logger_.Info("[ERROR] Expecting the game entity log.");
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
                    logger_.Info("[ERROR] Expecting the player entity log.");
                    yield break;
                }

                var match = PlayerEntityRegex.Match(this.parsing_log);
                var id = int.Parse(match.Groups["id"].Value);
                if (this.game_state.Entities.ContainsKey(id))
                {
                    logger_.Info("[ERROR] entity already exists.");
                    yield break;
                }
                this.game_state.Entities.Add(id, new Game.Entity(id));
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
            var match = CreationTagRegex.Match(this.parsing_log);
            if (!match.Success) return false;
            this.game_state.ChangeTag(entity_id, match.Groups["tag"].Value, match.Groups["value"].Value);
            return true;
        }

        private IEnumerable<bool> ParseTagChange()
        {
            var match = TagChangeRegex.Match(this.parsing_log);
            if (!match.Success) yield break;

            var entity_raw = match.Groups["entity"].Value;
            string entity_str;
            int entityId = ParserUtilities.GetEntityIdFromRawString(this.game_state, entity_raw, out entity_str);

            if (entityId >= 0) this.game_state.ChangeTag(entityId, match.Groups["tag"].Value, match.Groups["value"].Value);
            else game_state.ChangeTag(entity_str, match.Groups["tag"].Value, match.Groups["value"].Value);

            yield return true;
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
                logger_.Info("[ERROR] Parse error: cannot find entity id. '" + this.parsing_log + "'");
                yield return false;
            }

            if (!this.game_state.Entities.ContainsKey(entityId))
                this.game_state.Entities.Add(entityId, new Game.Entity(entityId));
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
                this.game_state.Entities.Add(id, new Game.Entity(id) { CardId = cardId });
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
                string entity_str;
                int entityId = ParserUtilities.GetEntityIdFromRawString(this.game_state, match.Groups["entity"].Value, out entity_str);
                if (entityId < 0) game_state.ChangeTag(entity_str, match.Groups["tag"].Value, match.Groups["value"].Value);
                else game_state.ChangeTag(entityId, match.Groups["tag"].Value, match.Groups["value"].Value);
                yield return true;
            }
        }

        private IEnumerable<bool> ParseMetadata()
        {
            if (!MetadataRegex.IsMatch(this.parsing_log)) yield break;
            yield return true;

            while (MetadataInfoRegex.IsMatch(this.parsing_log)) yield return true;
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
            if (entity_id < 0)
            {
                logger_.Info(String.Format("[INFO] Cannot get entity id '{0}'.", entity_raw));
            }

            var target_id = ParserUtilities.GetEntityIdFromRawString(this.game_state, target_raw);
            if (target_id < 0)
            {
                logger_.Info(String.Format("[INFO] Cannot get target id '{0}'.", target_raw));
            }

            if (this.BlockStart != null) this.BlockStart(this, new BlockStartEventArgs(entity_id, block_type));

            yield return true;

            while (true)
            {
                if (BlockEndRegex.IsMatch(this.parsing_log)) break;

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

                if (!matched) yield break;
            }

            if (this.BlockEnd != null) this.BlockEnd(this, new BlockEndEventArgs(entity_id, block_type));

            yield return true;
        }
    }
}
