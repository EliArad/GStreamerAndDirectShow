using DirectShowLib;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace UDPPlayerApp
{
    public class UDPPlayerNoDemux : BasePlayer
    {

         public UDPPlayerNoDemux(Form form, Panel panel, Callback p)
            : base(form, panel, p)
        {
            AddUDPFilter();           
            var t = new Thread(ConnectFilters);
            t.Start();
        }

         protected override void ConnectFilters()
         {
             int hr;
             m_form.Invoke((MethodInvoker)delegate()
             {
                 try
                 {
                     AddElecardAVCDecoder();

                     IPin pinUdpOut = DsFindPin.ByName(m_UdpFilter, "Out");
                     IPin pinDecoderXInput = DsFindPin.ByName(m_ElecardAVCDecoderFilter, "XForm In");
                     IPin pinDecoderXOutput = DsFindPin.ByName(m_ElecardAVCDecoderFilter, "XForm Out");

                     hr = m_graphBuilder.Connect(pinUdpOut, pinDecoderXInput);
                     DsError.ThrowExceptionForHR(hr);

                     hr = m_graphBuilder.Render((DirectShowLib.IPin)pinDecoderXOutput);
                     SetVideoWindow();
                     pCallback(10, "start");
                     return;
                 }
                 catch (Exception err)
                 {

                 }
             });
         }
    }
}
