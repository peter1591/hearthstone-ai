using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GameEngineUI
{
    public partial class Form1 : Form
    {
        private GameEngine engine_;
        private Logger logger_;

        public Form1()
        {
            InitializeComponent();

            logger_ = new Logger(ref this.textBox1);
            engine_ = new GameEngine(logger_);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            int ret = engine_.Run(10, 4);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            engine_.output_message_cb += (System.String msg) =>
            {
                logger_.Info("[Engine] " + msg);
            };

            logger_.Info("Initializing engine...");
            engine_.Initialize();
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            engine_.Destroy();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (!engine_.IsInitialized()) return;

            if (engine_.IsRunning())
            {

            }
        }
    }
}
