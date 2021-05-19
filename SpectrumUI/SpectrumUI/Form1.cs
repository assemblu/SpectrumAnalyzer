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

        string selectedPort;
        int ticks = 0;
        double[] freq = new double[10];
        double[] dataPoints = new double[10];
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
          //  System.Media.SystemSounds.Exclamation.Play();
            MessageBox.Show(abtMsg, "About");
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // Add serial ports to drop down menu
            updatePortItems();
            // Setup serial port
            serialPort1.BaudRate = 115200;
            serialPort1.Parity = Parity.None;
            serialPort1.DataBits = 8;
            serialPort1.StopBits = StopBits.One;
            serialPort1.Handshake = Handshake.None;

            double[] xs = { 31.5, 63, 100, 200, 500, 1000, 2500, 5000, 10000, 15100 };
            double[] ys = { 40,40,100,100,150,255,150,100,40,40 };
            var p=plotWindow.plt.PlotBar(xs,ys);
            p.barWidth = 10;
            p.showValues = true;
            plotWindow.Render();
            timer1.Stop();
        }
    
        void updatePortItems()
        {
            // Add available COM ports to drop down list
            portToolStripMenuItem.DropDown.Items.Clear();
            foreach ( string ports in SerialPort.GetPortNames())
            {
                var port = portToolStripMenuItem.DropDown.Items.Add(ports);
                port.Click += ComSelection;
            }
        }

        private void updatePlot()
        {
            for (int i = 0; i < 10; i++)
            {
                //MessageBox.Show("Data point value: " + dataPoints[i].ToString() + " at " + freq[i] + "Hz");
            }
            plotWindow.Reset();
            var p = plotWindow.plt.PlotBar(freq, dataPoints);
            p.barWidth = 10;
            p.showValues = true;
            plotWindow.Render();
        }

        private void quitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void savePlotToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Saves plot to file
            SaveFileDialog s = new SaveFileDialog();
            s.Filter = "PNG image file | *.png";
            s.AddExtension = true;
            s.DefaultExt = "png";
            if (s.ShowDialog() == DialogResult.OK)
            {
                MessageBox.Show(s.FileName);
                int[] dim = { plotWindow.Size.Width, plotWindow.Size.Height }; // Save original dimensions
                plotWindow.plt.Resize(4000, 1000); // Resize to output settings
                plotWindow.plt.SaveFig(s.FileName); // Save plot
                plotWindow.plt.Resize(dim[0], dim[1]); // Resize back to original size
            }
        }
        private void ComSelection(object sender, EventArgs e)
        {
            // This function runs whenever a COM port was selected from the menu
            ToolStripMenuItem selected = (ToolStripMenuItem)sender;
            selected.Checked = true;
            selectedPort = selected.Text;
            if (serialPort1.IsOpen) serialPort1.Close();
            serialPort1.PortName = selectedPort;
            serialPort1.Open();
            serialPort1.DiscardInBuffer();
            timer1.Start();
        }
        private void reOpenComPort()
        {
            if(serialPort1.IsOpen)
            {
                serialPort1.Close();
                serialPort1.Open();
                serialPort1.DiscardInBuffer();
                timer1.Start();
            }
        }
        private void resizeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Open new resize dialog
            SettingsDialog s = new SettingsDialog("Resize");
            s.ShowDialog();
            if( s.DialogResult==DialogResult.OK)
            {
                this.Size = new Size(s.newWidth , s.newHeight);

            }
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            
            // Data event
            int length = serialPort1.BytesToRead;



            if (length == 15 || length == 25)
            {
                ticks = 0;
                byte[] data = new byte[length];
                //for (int i = 0; i < length; i++)
                //{
                //    data[i] = serialport1.readbyte();
                //}
                serialPort1.Read(data, 0, length);
                if (data[0] == 0xFF && data[1] == 0x02 && data[length - 2] == 0xFF && data[length - 1] == 0x04) // If start and end is valid
                {
                    if (data[2] == 0x11) // Data is a range
                    {
                        int j = 0;
                        for (int i = 3; i < length - 2; i += 2)
                        {
                            int value16 = data[i] * 16 * 16 + data[i + 1];
                            
                            freq[j] = (24000 / 2048) * value16;
                            j++;
                        }
                        label1.BeginInvoke((MethodInvoker)delegate () { label1.Text = "Range updated!"; });
                    }
                    if (data[2] == 0x12) // Data are analyzer values
                    {
                        int j = 0;
                        for (int i = 3; i < length - 2; i++) // read values
                        {
                            dataPoints[j] = data[i];
                            j++;
                        }
                        updatePlot();
                        label1.BeginInvoke((MethodInvoker)delegate () { label1.Text = "Data updated!"; });
                    }
                }
            }
            else
            {
                
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            // tick interval 100 ms
            ticks += 1;
            if(ticks>50)
            {
                timer1.Stop();
                // If no update happenend for 5 sec, reset com port
                reOpenComPort();
                ticks = 0;
            }
        }
    }
}
