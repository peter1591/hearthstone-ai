using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.LogWatcher
{
    class LogWatcher
    {
        LogReader log_reader;

        public event EventHandler<GameState.StartWaitingMainActionEventArgs> StartWaitingMainAction;
        public event EventHandler<LogParser.ActionStartEventArgs> ActionStart;
        public event EventHandler<LogParser.EndTurnEventArgs> EndTurnEvent;
        public event EventHandler<SubParsers.PowerLogParser.CreateGameEventArgs> CreateGameEvent;

        public delegate void LogMsgDelegate(String msg);
        public LogMsgDelegate log_msg;

        public delegate void UpdateBoardDelegate(GameState game_state, Board.Game board, String err_msg = "");
        public UpdateBoardDelegate update_board;

        public void Reset(string hearthstone_path)
        {
            this.log_reader = new LogReader(hearthstone_path);
            this.log_reader.StartWaitingMainAction += StartWaitingMainAction;
            this.log_reader.ActionStart += ActionStart;
            this.log_reader.EndTurnEvent += EndTurnEvent;
            this.log_reader.CreateGameEvent += CreateGameEvent;
            this.log_reader.log_msg = (string msg) => log_msg(msg);
        }

        public void Tick()
        {
            this.log_reader.Process();
            this.UpdateBoardIfNecessary();
        }

        private int last_invoke_log_change_id = -1;
        private int last_stable_change_id = -1;
        private DateTime last_stable_time = DateTime.Now;
        private TimeSpan stable_time_to_invoke_AI = new TimeSpan(0, 0, 0, 0, 100);
        private Board.Game last_invoke_board = new Board.Game();
        private void UpdateBoardIfNecessary()
        {
            int current_change_id = this.log_reader.GetChangeId();

            if (current_change_id != last_stable_change_id)
            {
                this.last_stable_change_id = current_change_id;
                this.last_stable_time = DateTime.Now;
                return;
            }

            if ((DateTime.Now - this.last_stable_time) < this.stable_time_to_invoke_AI)
            {
                // not stable enough
                return;
            }

            // now the log is stable enough
            // --> invoke AI if we didn't do it yet
            if (this.last_invoke_log_change_id == current_change_id)
            {
                // we've already invoked the AI for this stable log
                return;
            }

            this.last_invoke_log_change_id = current_change_id;

            var game = this.log_reader.GetGameState();

            Board.Game board = new Board.Game();
            bool parse_success = board.Parse(this.log_reader.GetGameState());

            if (parse_success == false)
            {
                update_board(log_reader.GetGameState(), null, "parse failed");
                return;
            }

            if (board.Equals(this.last_invoke_board))
            {
                //this.AddLog("Log changed, but board are the same as the last-invoked one -> skipping");
                return;
            }
            this.last_invoke_board = board;

            update_board(log_reader.GetGameState(), board);
        }
    }
}
