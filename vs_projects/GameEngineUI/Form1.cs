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
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var ofd = new OpenFileDialog
            {
                CheckFileExists = true,
                Filter = "Native Library|GameEngineCppWrapper.dll",
                InitialDirectory = Environment.CurrentDirectory
            };

            var result = ofd.ShowDialog(this);
            if (result != DialogResult.OK) return;

            GameEngineCppWrapper.CLI.GameEngine.InitializeLibrary(ofd.FileName);

            GameEngineCppWrapper.CLI.GameEngine engine = new GameEngineCppWrapper.CLI.GameEngine();
            int v = engine.Get();
            MessageBox.Show(v.ToString());
        }
    }
}
