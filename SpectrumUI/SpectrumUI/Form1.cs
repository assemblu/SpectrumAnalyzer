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
using ScottPlot.plottables;

namespace SpectrumUI
{
    public partial class Form1 : Form
    {

        string selectedPort;
        int ticks = 0;
        double[] freq = new double[512];
        double[] dataPoints = new double[512];
        ScottPlot.PlottableBar pSignal;
        int j = 0;
        public Form1()
        {
            InitializeComponent();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            
            string abtMsg = @"For source code visit github.com/<whatever>
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


            Array.Clear(dataPoints,0,512);
            for(int i=1; i<=512; i++)
            {
                freq[i - 1] = (24000 / 512) * i;
            }
            pSignal = plotWindow.plt.PlotBar(freq, dataPoints);
            plotWindow.plt.AxisBounds(0, 20000, 0, 255);
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
            //plotWindow.Reset();
            pSignal.ys = dataPoints;
            pSignal.xs = freq;
            plotWindow.Render(skipIfCurrentlyRendering: true);
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
            if (serialPort1.IsOpen) serialPort1.Close();
            try
            {
                selected.Checked = true;
                selectedPort = selected.Text;
                serialPort1.PortName = selectedPort;
                serialPort1.Open();
                serialPort1.DiscardInBuffer();
                timer1.Start();
            }
            catch
            {
                MessageBox.Show("Error opening port", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
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
            if(length >= 517)
            {
                byte[] dataIn = new byte[length];
                serialPort1.Read(dataIn, 0, length);
                int ind = Array.IndexOf(dataIn, (byte)0xFF);
                if (ind >= 0)
                {

                    //If beginning was found
                    byte[] data;
                    //if (dataIn[ind] == 0xFF && dataIn[ind + 1] == 0x02 && dataIn[ind + 2] == 0x11)
                    //{
                    //    // Expecting range
                    //    data = new byte[20];
                    //    // Copy 20 bytes
                    //    Array.Copy(dataIn, ind + 2, data, 0, 20);
                    //    int j = 0;
                    //    for (int i = 0; i < 10; i++)
                    //    {
                    //        int value16 = data[i] * 16 * 16 + data[i + 1];
                    //        freq[j] = (24000 / 512) * value16;
                    //        j++;
                    //    }
                    //    label1.BeginInvoke((MethodInvoker)delegate () { label1.Text = "Range updated!"; });
                    //}
                    if (dataIn[ind] == 0xFF && dataIn[ind + 1] == 0x02 && dataIn[ind + 2] == 0x12)
                    {
                        // Expecting data
                        //data = new byte[10];
                        // Copy 10 bytes
                        //Array.Copy(dataIn, ind + 3, data, 0, 10);
                        //int j = 0;
                        //for (int i = 0; i < 10; i++) // read values
                        //{
                        //    dataPoints[j] = (double)data[i];
                        //    j++;
                        //}
                        //dataPoints[j] = (double)dataIn[ind + 3];
                        int j = 0;
                        for(int i=ind+3; i<(ind+3+512); i++)
                        {
                            dataPoints[j] = (double)dataIn[i];
                            j++;
                        }
                        label1.BeginInvoke((MethodInvoker)delegate () { label1.Text = "Data updated!"; });
                        updatePlot();
                        serialPort1.WriteLine("");
                    }
                    else if (dataIn[ind] == 0xFF && dataIn[ind + 1] == 0x02 && dataIn[ind + 2] == 0x13 && dataIn[ind + 3] == 0xFF && dataIn[ind + 4] == 0x04)
                    {
                        j = 0;
                        serialPort1.DiscardInBuffer();
                        // updatePlot();
                    }
                    ticks = 0;
                }
            }
            //  byte[] data = new byte[length];
            //for (int i = 0; i < length; i++)
            //{
            //    data[i] = serialport1.readbyte();
            //}
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

        private void button1_Click(object sender, EventArgs e)
        {
            serialPort1.WriteLine("");
        }
    }
}
