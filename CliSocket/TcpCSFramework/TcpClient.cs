using System;
using System.Collections.Generic;
using System.Text;
using TcpCSFramework;

namespace TcpClient
{
    /// <summary>
    /// Client
    /// </summary>
    public class TcpClient
    {
        public TcpClient()
        {
            //
            // TODO: 在此处添加构造函数逻辑
            //
        }
        public static void Main()
        {
            Console.WriteLine("Begin to Test TcpCli Class..");
            //ArrayList al = new ArrayList();

            TcpClient test = new TcpClient();
            TcpCli cli1 = new TcpCli(new Coder(Coder.EncodingMothord.Default));

            //cli1.Resovlver = new DatagramResolver("]}");
            cli1.ReceivedDatagram += new NetEvent(test.RecvData);
            cli1.DisConnectedServer += new NetEvent(test.ClientClose);
            cli1.ConnectedServer += new NetEvent(test.ClientConn);
            cli1.Connect("127.0.0.1", 5001);
            //命令控制循环
            while (true)
            {
                Console.Write(">");
                string cmd = Console.ReadLine();
            }//end of while
        }

        void ClientConn(object sender, NetEventArgs e)
        {
            string info = string.Format("A Client:{0} connect server :{1}", e.Client,
             e.Client.ClientSocket.RemoteEndPoint.ToString());

            Console.WriteLine(info);
            Console.Write(">");
        }

        void ClientClose(object sender, NetEventArgs e)
        {
            string info;

            if (e.Client.TypeOfExit == Session.ExitType.ExceptionExit)
            {
                info = string.Format("A Client Session:{0} Exception Closed.",
                 e.Client.ID);
            }
            else
            {
                info = string.Format("A Client Session:{0} Normal Closed.",
                 e.Client.ID);
            }

            Console.WriteLine(info);
            Console.Write(">");
        }

        void RecvData(object sender, NetEventArgs e)
        {
            ushort cmd=0;
            e.Client.RecvPacket.ReadUShort(ref cmd);
            char bo='\0';
            e.Client.RecvPacket.ReadByte(ref bo);

            string str = Encoding.Default.GetString(e.Client.RecvPacket.GetReadData());
            string info = string.Format("recv data:{0}", cmd);
            Console.WriteLine(info);
            Console.Write(">");
        }
    }
}
