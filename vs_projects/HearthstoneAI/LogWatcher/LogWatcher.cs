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

        public delegate void LogMsgDelegate(String msg);
        public LogMsgDelegate log_msg;

        public void Reset(string hearthstone_path)
        {
            game_state_ = new GameState();
            this.stable_decider_ = new StableDecider(100); // 100 ms

            this.log_reader = new LogReader(hearthstone_path, game_state_);
            game_state_.StartWaitingMainAction += StartWaitingMainAction;
            this.log_reader.ActionStart += (sender, e) =>
            {
                if (this.ActionStart != null) this.ActionStart(this, new ActionStartEventArgs(e, game_state_));
            };
            this.log_reader.CreateGameEvent += CreateGameEvent;
            this.log_reader.log_msg = (string msg) => log_msg(msg);
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
