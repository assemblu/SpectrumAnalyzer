using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace SpectrumUI
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            
            string abtMsg = @"Written by the spectrum analyzer group of 2021.
Moritz Breuer (EPA), Peter van Breugel (EIE), Emirhan Gocturk (ACS), Stef Mebius (EIE)
For source code visit github.com/<whatever>
";
            System.Media.SystemSounds.Exclamation.Play();
            MessageBox.Show(abtMsg, "About");
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            double[] xs = { 31.5, 63, 100, 200, 500, 1000, 2500, 5000, 10000, 15100 };
            double[] ys = { 40,40,100,100,150,255,150,100,40,40 };
            var p=plotWindow.plt.PlotBar(xs,ys);
            p.barWidth = 10;
            p.showValues = true;
            plotWindow.Render();
        }
    
        void updatePortItems()
        {
            portToolStripMenuItem.DropDown.Items.Clear();
            foreach ( string ports in SerialPort.GetPortNames())
            {
                portToolStripMenuItem.DropDown.Items.Add(ports);
            }
        }

        private void portToolStripMenuItem_MouseHover(object sender, EventArgs e)
        {
            updatePortItems();
        }

        private void quitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void savePlotToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog s = new SaveFileDialog();
            s.Filter = "PNG image file | *.png";
            s.AddExtension = true;
            s.DefaultExt = "png";
            if (s.ShowDialog() == DialogResult.OK)
            {
                MessageBox.Show(s.FileName);
                int[] dim = { plotWindow.Size.Width, plotWindow.Size.Height };
                plotWindow.plt.Resize(4000, 1000);
                plotWindow.plt.SaveFig(s.FileName);
                plotWindow.plt.Resize(dim[0], dim[1]);
            }
        }

        private void resizeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SettingsDialog s = new SettingsDialog("Resize");
            s.ShowDialog();
            if( s.DialogResult==DialogResult.OK)
            {
                this.Size = new Size(s.newWidth , s.newHeight);

            }
        }
    }
}
