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
        static int dataLength = 1668; // The last value in the received data that is actually useful
        static int totalLength = 2048; // The length of the fft result

        string selectedPort;
        int ticks = 0;
        double[] freq = new double[dataLength];
        double[] dataPoints = new double[dataLength];
        ScottPlot.PlottableBar pSignal;
        int j = 0;
        public Form1()
        {
            InitializeComponent();
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {

            string abtMsg = @"For source code visit https://github.com/emirgo/SpectrumAnalyzer
";
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

	    // Fill arrays with zeros and add them to plot
            Array.Clear(dataPoints,0,dataPoints.Length);
            Array.Clear(freq, 0, freq.Length);
            for (int i=1; i<=dataLength; i++)
            {
                freq[i - 1] = (20000.0 / dataLength) * i;
            }
            pSignal = plotWindow.plt.PlotBar(freq, dataPoints);
            plotWindow.plt.AxisBounds(0, 20000, 0, 255);
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

        void selectFirstPort()
        {
            if (selectedPort == "")
            {
                if (SerialPort.GetPortNames().Length == 1)
                {
                    if (!serialPort1.IsOpen)
                    {
                        selectedPort = SerialPort.GetPortNames()[0];
                        serialPort1.PortName = selectedPort;
                        serialPort1.Open();
                        serialPort1.DiscardInBuffer();
                    }
                }
                else
                {
                    MessageBox.Show("Please select a port first!", "Info", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
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

            // Data received event

            int length = serialPort1.BytesToRead; // Length of received data
            if(length >= totalLength+5) // if length is valid
            {
                byte[] dataIn = new byte[length];
                serialPort1.Read(dataIn, 0, length); // Read in data from buffer
                int ind = Array.IndexOf(dataIn, (byte)0xFF); // Find index of the beginning in the buffer
                if (ind >= 0)
                {

                    //If beginning was found
                    byte[] data;
                    if (dataIn[ind] == 0xFF && dataIn[ind + 1] == 0x02 && dataIn[ind + 2] == 0x12) // Check if start and control bytes are valid
                    {
                        // Expecting data
                        int j = 0;
			// Extract data values from buffer
                        for(int i=ind+3; i<(ind+3+dataLength); i++)
                        {
                            dataPoints[j] = (double)dataIn[i];
                            j++;
                        }
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
        }

        private void button1_Click(object sender, EventArgs e)
        {
            selectFirstPort();
            if(serialPort1.IsOpen) serialPort1.WriteLine("");
            plotWindow.plt.Axis(0, 20000, 0, 255);
        }
    }
}
