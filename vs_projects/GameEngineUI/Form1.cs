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
        GameEngineCppWrapper.CLI.GameEngine engine_;

        public Form1()
        {
            InitializeComponent();

            var ofd = new OpenFileDialog
            {
                CheckFileExists = true,
                Filter = "Native Library|GameEngineCppWrapper.dll",
                InitialDirectory = Environment.CurrentDirectory
            };

            //var result = ofd.ShowDialog(this);
            //if (result != DialogResult.OK) return;
            //
            //GameEngineCppWrapper.CLI.GameEngine.InitializeLibrary(ofd.FileName);

            engine_ = new GameEngineCppWrapper.CLI.GameEngine();
            engine_.Initialize();
        }

        ~Form1()
        {
            if (engine_ != null)
            {
                engine_.Destroy();
            }
        }

        private void RunAI()
        {
            engine_.Run(60, 4);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            System.Threading.Thread new_thread = new System.Threading.Thread(this.RunAI);
            new_thread.Start();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
    }
}
