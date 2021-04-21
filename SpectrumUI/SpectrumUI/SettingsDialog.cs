using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace SpectrumUI
{
    public partial class SettingsDialog : Form
    {
        public int newWidth { get; set; }
        public int newHeight { get; set; }
        public SettingsDialog(string windowName)
        {
            InitializeComponent();
            this.Text = windowName;
            if (windowName == "Resize")
            {
                button1.Text = "Resize";
            }
        }

        private void SettingsDialog_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.newWidth = Convert.ToUInt16(width.Text);
            this.newHeight = Convert.ToUInt16(height.Text);
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void SettingsDialog_FormClosing(object sender, FormClosingEventArgs e)
        {
            if(this.DialogResult!=DialogResult.OK) this.DialogResult = DialogResult.Cancel;
        }
    }
}
