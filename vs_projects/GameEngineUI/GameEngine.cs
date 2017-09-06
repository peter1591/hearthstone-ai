using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameEngineUI
{
    class GameEngine
    {
        public Logger logger_;

        private GameEngineCppWrapper.CLI.GameEngine.OutputMessageCallback _output_message_cb;
        public GameEngineCppWrapper.CLI.GameEngine.OutputMessageCallback output_message_cb
        {
            get
            {
                return _output_message_cb;
            }
            set
            {
                _output_message_cb += value;
                engine_.SetOutputMessageCallback(_output_message_cb);
            }
        }

        public GameEngine(Logger logger)
        {
            engine_ = new GameEngineCppWrapper.CLI.GameEngine();
            logger_ = logger;
        }

        private bool initialized_ = false;
        public void Initialize()
        {
            new System.Threading.Thread(() =>
            {
                engine_.Initialize();
                initialized_ = true;
                logger_.Info("Engine initialized.");
            }).Start();
        }
        public bool IsInitialized() { return initialized_; }

        public bool IsRunning() { return (runner_ != null); }

        public int Run(int seconds, int threads)
        {
            if (!IsInitialized()) return -1;

            if (runner_ != null)
            {
                if (runner_.IsAlive) return -1;
            }

            runner_ = new System.Threading.Thread(() =>
            {
                logger_.Info(String.Format("Start run for {0} seconds with {1} threads.",
                    seconds, threads));
                engine_.Run(seconds, threads);
                logger_.Info("Finish run.");
            });
            runner_.Start();

            return 0;
        }

        public void Destroy()
        {
            if (runner_ != null && runner_.IsAlive)
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
