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

        public event EventHandler<GameState.StartWaitingMainActionEventArgs> StartWaitingMainAction;
        public event EventHandler<LogParser.ActionStartEventArgs> ActionStart;
        public event EventHandler<LogParser.EndTurnEventArgs> EndTurnEvent;
        public event EventHandler<SubParsers.PowerLogParser.CreateGameEventArgs> CreateGameEvent;

        public delegate void LogMsgDelegate(String msg);
        public LogMsgDelegate log_msg;

        public delegate void BoardChangeDelegate(GameState game_state, Board.Game board, String err_msg = "");
        public BoardChangeDelegate board_changed;

        public void Reset(string hearthstone_path)
        {
            this.stable_decider_ = new StableDecider(1000); // 1000 ms

            this.log_reader = new LogReader(hearthstone_path);
            this.log_reader.StartWaitingMainAction += StartWaitingMainAction;
            this.log_reader.ActionStart += ActionStart;
            this.log_reader.EndTurnEvent += EndTurnEvent;
            this.log_reader.CreateGameEvent += CreateGameEvent;
            this.log_reader.log_msg = (string msg) => log_msg(msg);
            this.log_reader.log_changed = () =>
            {
                stable_decider_.Changed();
            };
        }

        public void Tick()
        {
            this.log_reader.Process();
            this.UpdateBoardIfNecessary();
        }

        private Board.Game last_invoke_board = new Board.Game();
        private void UpdateBoardIfNecessary()
        {
            if (!stable_decider_.IsStable()) return;

            var game = this.log_reader.GetGameState(); // TODO: allocate game state in member

            Board.Game board = new Board.Game();
            bool parse_success = board.Parse(this.log_reader.GetGameState());

            if (parse_success == false)
            {
                board_changed(log_reader.GetGameState(), null, "parse failed");
                return;
            }

            if (board.Equals(this.last_invoke_board))
            {
                return;
            }
            this.last_invoke_board = board;

            board_changed(log_reader.GetGameState(), board);
        }
    }
}
