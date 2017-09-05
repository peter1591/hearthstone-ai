using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameEngineUI
{
    class GameEngine
    {
        public GameEngine()
        {
            engine_ = new GameEngineCppWrapper.CLI.GameEngine();
            engine_.Initialize();
        }

        public bool IsRunning() { return (runner_ != null); }

        public int Run(int seconds, int threads)
        {
            if (runner_ != null)
            {
                if (runner_.IsAlive) return -1;
            }

            runner_ = new System.Threading.Thread(() =>
            {
                engine_.Run(seconds, threads);
            });
            runner_.Start();

            return 0;
        }

        public void Destroy()
        {
            if (runner_.IsAlive)
            {
                // TODO: notify engine_ to early-stop
                runner_.Join();
            }

            engine_ = null;
        }

        private GameEngineCppWrapper.CLI.GameEngine engine_;
        private System.Threading.Thread runner_ = null;
    }
}
