using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Network
{
    class Program
    {
        static void Main(string[] args)
        {
            TcpSession client=new TcpSession();
            client.Connect("127.0.0.1", 8885);
            while (true)
            {
                string say=Console.ReadLine();
                if (say.Length > 0)
                {
                    CmdPacket packet = new CmdPacket();
                    packet.BeginWrite();
                    packet.WriteShort(22);
                    packet.WriteString(say);
                    client.SendPacket(packet);
                }
            }
        }
    }
}
