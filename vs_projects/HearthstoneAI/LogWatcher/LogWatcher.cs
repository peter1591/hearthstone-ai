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
        private GameState game_state_;
        private Logger logger_;

        public LogWatcher(Logger logger)
        {
            logger_ = logger;
        }

        public event EventHandler<GameState.StartWaitingMainActionEventArgs> StartWaitingMainAction;

        public class ActionStartEventArgs : SubParsers.PowerLogParser.ActionStartEventArgs
        {
            public ActionStartEventArgs(SubParsers.PowerLogParser.ActionStartEventArgs e, GameState game) : base(e)
            {
                this.game = game;
            }

            public GameState game;
        };
        public event EventHandler<ActionStartEventArgs> ActionStart;

        public class EndTurnEventArgs : GameState.EndTurnEventArgs
        {
            public EndTurnEventArgs(GameState.EndTurnEventArgs e, GameState game) : base(e)
            {
                this.game = game;
            }

            public GameState game;
        };
        public event EventHandler<EndTurnEventArgs> EndTurnEvent;

        public event EventHandler<SubParsers.PowerLogParser.CreateGameEventArgs> CreateGameEvent;

        public void Reset(string hearthstone_path)
        {
            game_state_ = new GameState();
            this.stable_decider_ = new StableDecider(100); // 100 ms

            this.log_reader = new LogReader(hearthstone_path, game_state_, logger_);
            game_state_.StartWaitingMainAction += StartWaitingMainAction;
            this.log_reader.ActionStart += (sender, e) =>
            {
                if (this.ActionStart != null) this.ActionStart(this, new ActionStartEventArgs(e, game_state_));
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
        public delegate void GameStateChangeDelegate(GameState game_state);
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
    }
}
