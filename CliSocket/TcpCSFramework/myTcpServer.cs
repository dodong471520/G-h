//using System;
//using TcpCSFramework;
//using System.Collections;
//using System.Net.Sockets;
//using System.Windows.Forms;


//namespace TCPServer
//{
//    /// <summary>
//    /// server
//    /// </summary>
//    public class myTcpServer
//    {
//        private Hashtable m_ChannelSock = new Hashtable();
        
//        public static void Main()
//        {
//            try
//            {
//                Console.WriteLine("TcpSvr is beginning to start...");
                
//                myTcpServer tts = new myTcpServer();
//                TcpSvr svr = new TcpSvr(6500, 1024, new Coder(Coder.EncodingMothord.Default));

//                svr.Resovlver = new DatagramResolver("]}");
//                //����ͻ��������������¼�
//                svr.ServerFull += new NetEvent(tts.ServerFull);
//                //�����¿ͻ��������¼�
//                svr.ClientConn += new NetEvent(tts.ClientConn);
//                //����ͻ��˹ر��¼�
//                svr.ClientClose += new NetEvent(tts.ClientClose);
//                //������յ������¼�
//                svr.RecvData += new NetEvent(tts.RecvData);
                
//                while (true)
//                {
//                    Console.Write(">");
//                    string cmd = Console.ReadLine();
//                    //�˳�����
//                    if (cmd.ToLower() == "exit")
//                    {
//                        break;
//                    }
//                    //��ͣ����
//                    if (cmd.ToLower() == "stop")
//                    {
//                        svr.Stop();
//                        Console.WriteLine("Server is Stop.");
//                        continue;
//                    }
//                    //��������
//                    if (cmd.ToLower() == "start")
//                    {
//                        svr.Start();
//                        Console.WriteLine("Server is listen...{0}", svr.ServerSocket.LocalEndPoint.ToString());
//                        continue;
//                    }
//                    //������
//                    if (cmd.ToLower() == "count")
//                    {
//                        Console.WriteLine("Current count of Client is {0}/{1}", svr.SessionCount, svr.Capacity);
//                        continue;
//                    }
//                    //�������ݵ��ͻ��˸�ʽ:send [Session] [stringData]
//                    if (cmd.ToLower().IndexOf("send") != -1)
//                    {
//                        cmd = cmd.ToLower();
//                        string[] para = cmd.Split(' ');
//                        if (para.Length == 3)
//                        {
//                            Session client = (Session)svr.SessionTable[new SessionId(int.Parse(para[1]))];

//                            if (client != null)
//                            {
//                                svr.Send(client, para[2]);
//                            }
//                            else
//                            {
//                                Console.WriteLine("The Session is Null");
//                            }
//                        }
//                        else
//                        {
//                            Console.WriteLine("Error Command");
//                        }

//                        continue;
//                    }

//                    //�ӷ��������ߵ�һ���ͻ��� ��ʽ�� kick [Session]
//                    if (cmd.ToLower().IndexOf("kick") != -1)
//                    {
//                        cmd = cmd.ToLower();

//                        string[] para = cmd.Split(' ');

//                        if (para.Length == 2)
//                        {
//                            Session client = (Session)svr.SessionTable[new SessionId(int.Parse(para[1]))];

//                            if (client != null)
//                            {
//                                svr.CloseSession(client);
//                            }
//                            else
//                            {
//                                Console.WriteLine("The Session is Null");
//                            }
//                        }
//                        else
//                        {
//                            Console.WriteLine("Error command");
//                        }

//                        continue;
//                    }

//                    //�г������������еĿͻ�����Ϣ
//                    if (cmd.ToLower() == "list")
//                    {
//                        int i = 0;

//                        foreach (Session Client in svr.SessionTable.Values)
//                        {
//                            if (Client != null)
//                            {
//                                i++;
//                                string info = string.Format("{0} Client:{1} connected server Session:{2}. Socket Handle:{3}",
//                                 i,
//                                 Client.ClientSocket.RemoteEndPoint.ToString(),
//                                 Client.ID,
//                                 Client.ClientSocket.Handle);

//                                Console.WriteLine(info);
//                            }
//                            else
//                            {
//                                i++;
//                                string info = string.Format("{0} null Client", i);
//                                Console.WriteLine(info);
//                            }
//                        }
//                        continue;

//                    }
//                    if (cmd.ToLower() == "reset")
//                    {
//                        continue;
//                    }
//                    Console.WriteLine("Unkown Command");

//                }//end of while

//                Console.WriteLine("End service");
//            }
//            catch (Exception ex)
//            {
//                Console.WriteLine(ex.ToString());
//            }
//        }
//        /// <summary>
//        /// �ͻ��˽�������
//        /// </summary>
//        /// <param name="sender"></param>
//        /// <param name="e"></param>
//        void ClientConn(object sender, NetEventArgs e)
//        {
//            string info = string.Format("A Client:{0} connect server Session:{1}. Socket Handle:{2}",
//             e.Client.ClientSocket.RemoteEndPoint.ToString(),
//             e.Client.ID, e.Client.ClientSocket.Handle);

//            Console.WriteLine(info);
//            Console.Write(">");
//        }
//        /// <summary>
//        /// �ͻ�������
//        /// </summary>
//        /// <param name="sender"></param>
//        /// <param name="e"></param>
//        void ServerFull(object sender, NetEventArgs e)
//        {
//            string info = string.Format("Server is full. The Client:{0} is refused",
//             e.Client.ClientSocket.RemoteEndPoint.ToString());

//            //Must do it
//            e.Client.Close();

//            Console.WriteLine(info);
//            Console.Write(">");
//        }
//        /// <summary>
//        /// �رտͻ���
//        /// </summary>
//        /// <param name="sender"></param>
//        /// <param name="e"></param>
//        void ClientClose(object sender, NetEventArgs e)
//        {
//            string info;

//            if (e.Client.TypeOfExit == Session.ExitType.ExceptionExit)
//            {
//                info = string.Format("A Client Session:{0} Exception Closed.",
//                 e.Client.ID);
//            }
//            else
//            {
//                info = string.Format("A Client Session:{0} Normal Closed.", e.Client.ID);
//            }

//            if (e.Client.Channel != "")
//            {
//                this.m_ChannelSock.Remove(e.Client.Channel);
//            }

//            Console.WriteLine(info);
//            Console.Write(">");
//        }
//        /// <summary>
//        /// ��������
//        /// </summary>
//        /// <param name="sender"></param>
//        /// <param name="e"></param>
//        void RecvData(object sender, NetEventArgs e)
//        {
//            //��ʾ���յ�������
//            Console.WriteLine("Received data: " + e.Client.Datagram + " From: " + e.Client);
//            Console.Write(">");
//        }
//    }
//}
