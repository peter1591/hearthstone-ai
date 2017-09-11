using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.LogWatcher
{
    class LogWatcher
    {
        private LogReader log_reader;
        private StableDecider stable_decider_;
        private Game game_state_;
        private Logger logger_;

        public LogWatcher(Logger logger)
        {
            logger_ = logger;
        }

        public event EventHandler<Game.StartWaitingMainActionEventArgs> StartWaitingMainAction;

        public class BlockStartEventArgs : SubParsers.PowerLogParser.BlockStartEventArgs
        {
            public BlockStartEventArgs(SubParsers.PowerLogParser.BlockStartEventArgs e, Game game) : base(e)
            {
                this.game = game;
            }

            public Game game;
        };
        public event EventHandler<BlockStartEventArgs> BlockStart;

        public class BlockEndEventArgs : SubParsers.PowerLogParser.BlockEndEventArgs
        {
            public BlockEndEventArgs(SubParsers.PowerLogParser.BlockEndEventArgs e, Game game) : base(e)
            {
                this.game = game;
            }

            public Game game;
        };
        public event EventHandler<BlockEndEventArgs> BlockEnd;

        public class EndTurnEventArgs : Game.EndTurnEventArgs
        {
            public EndTurnEventArgs(Game.EndTurnEventArgs e, Game game) : base(e)
            {
                this.game = game;
            }

            public Game game;
        };
        public event EventHandler<EndTurnEventArgs> EndTurnEvent;

        public event EventHandler<SubParsers.PowerLogParser.CreateGameEventArgs> CreateGameEvent;

        public void Reset(string hearthstone_path)
        {
            game_state_ = new Game();
            this.stable_decider_ = new StableDecider(100); // 100 ms

            this.log_reader = new LogReader(hearthstone_path, game_state_, logger_);
            game_state_.StartWaitingMainAction += StartWaitingMainAction;
            this.log_reader.BlockStart += (sender, e) =>
            {
                if (this.BlockStart != null) this.BlockStart(this, new BlockStartEventArgs(e, game_state_));
            };
            this.log_reader.BlockEnd += (sender, e) =>
            {
                if (e.block_type == "PLAY")
                {
                    AnalyzePlayHandCardAction(e.entity_id);
                }

                if (this.BlockEnd != null) this.BlockEnd(this, new BlockEndEventArgs(e, game_state_));
            };
            this.log_reader.CreateGameEvent += (sender, e) =>
            {
                // a CREATE_GAME will present for every 30 minutes
                // or maybe when you re-connected to the game?
                // So here we don't reset the game unless the Game State is COMPLETE
                if (game_state_.GameEntityId > 0)
                {
                    var game_entity = game_state_.Entities[game_state_.GameEntityId];
                    if (game_entity.GetTagOrDefault(GameTag.STATE, (int)TAG_STATE.RUNNING) != (int)TAG_STATE.COMPLETE)
                    {
                        return;
                    }
                }

                game_state_.Reset();

                CreateGameEvent(sender, e);
            };
            this.log_reader.log_changed = () =>
            {
                stable_decider_.Changed();
            };

            game_state_.EndTurnEvent += (sender, e) =>
            {
                if (this.EndTurnEvent != null) this.EndTurnEvent(this, new EndTurnEventArgs(e, game_state_));
            };
        }

        private bool tick_processing_ = false;
        public delegate void GameStateChangeDelegate(Game game_state);
        public GameStateChangeDelegate game_state_changed;
        public void Tick()
        {
            if (tick_processing_) return;
            tick_processing_ = true;

            this.log_reader.Process();

            if (stable_decider_.IsStable())
            {
                game_state_changed(game_state_);
            }

            tick_processing_ = false;
        }

        private void AnalyzePlayHandCardAction(int entity_id)
        {
            // get current player
            int playing_entity = this.GetPlayingPlayerEntityID();
            if (playing_entity < 0) return;

            string playing_card = game_state_.Entities[entity_id].CardId.ToString();

            if (playing_entity == game_state_.PlayerEntityId)
            {
                game_state_.player_played_hand_cards.Add(playing_card);
            }
            else if (playing_entity == game_state_.OpponentEntityId)
            {
                game_state_.opponent_played_hand_cards.Add(playing_card);
            }
            else return;
        }

        private int GetPlayingPlayerEntityID()
        {
            Game.Entity game_entity;
            if (!game_state_.TryGetGameEntity(out game_entity)) return -1;

            Game.Entity player_entity;
            if (!game_state_.TryGetPlayerEntity(out player_entity)) return -1;

            Game.Entity opponent_entity;
            if (!game_state_.TryGetOpponentEntity(out opponent_entity)) return -1;

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
