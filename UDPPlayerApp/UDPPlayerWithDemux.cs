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
    public class UDPPlayerWithDemux : BasePlayer
    {

        public UDPPlayerWithDemux(Form form, Panel panel, Callback p)
            : base(form, panel, p)
        {
            AddUDPFilter();
            AddElecardMPEGPushDemux();
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
                    IPin pinUdpOut = DsFindPin.ByName(m_UdpFilter, "Out");
                    IPin pinPushDemuxIn = DsFindPin.ByName(m_ElecardPushMPEGDemuxFilter, "Input");

                    hr = m_graphBuilder.Connect(pinUdpOut, pinPushDemuxIn);
                    DsError.ThrowExceptionForHR(hr);


                    IPin pinPushDemuxOut = null;
                    while (m_running && pinPushDemuxOut == null)
                    {
                        pinPushDemuxOut = DsFindPin.ByName(m_ElecardPushMPEGDemuxFilter, "AVC");
                        Thread.Sleep(400);
                    }
                    if (m_running == false)
                        return;
                    AddElecardAVCDecoder();
                    hr = m_graphBuilder.Render((DirectShowLib.IPin)pinPushDemuxOut);
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
