using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System;
using System.IO.Ports;
using System.Threading;

namespace UIUI
{
    public partial class Form1 : Form
    {
        SerialPort uart = null;

        public Form1()
        {
            InitializeComponent();
        }

    }
}
