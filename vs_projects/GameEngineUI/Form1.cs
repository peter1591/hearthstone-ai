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

        public Form1()
        {
            InitializeComponent();
        }

        private void RunAI()
        {
            MessageBox.Show(engine_.Run(10, 4).ToString());
        }

        private void button2_Click(object sender, EventArgs e)
        {
            System.Threading.Thread new_thread = new System.Threading.Thread(this.RunAI);
            new_thread.Start();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            engine_ = new GameEngine();
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            engine_.Destroy();
        }
    }
}
