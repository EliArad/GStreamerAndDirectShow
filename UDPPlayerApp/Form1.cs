using DirectShowLib;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace UDPPlayerApp
{
    public partial class Form1 : Form
    {
  
        
        BasePlayer m_player;
        public Form1()
        {
            InitializeComponent();             
        } 
        
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                m_player.Close();
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message);
            }
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            m_player.Resize();
        }

        void Callback(int code, object msg)
        {
            switch (code)
            {
                case 10:
                    {
                        m_player.Run();
                    }
                    break;
            }
        }
        private void Form1_Load(object sender, EventArgs e)
        {
            try
            {
                BasePlayer.Callback p = new BasePlayer.Callback(Callback);
                m_player = new UDPPlayerNoDemux(this, panel1, p);
               
                                
            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message);
            }
        }
        
    }
}
