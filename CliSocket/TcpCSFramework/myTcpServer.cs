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
//                //处理客户端连接数已满事件
//                svr.ServerFull += new NetEvent(tts.ServerFull);
//                //处理新客户端连接事件
//                svr.ClientConn += new NetEvent(tts.ClientConn);
//                //处理客户端关闭事件
//                svr.ClientClose += new NetEvent(tts.ClientClose);
//                //处理接收到数据事件
//                svr.RecvData += new NetEvent(tts.RecvData);
                
//                while (true)
//                {
//                    Console.Write(">");
//                    string cmd = Console.ReadLine();
//                    //退出服务
//                    if (cmd.ToLower() == "exit")
//                    {
//                        break;
//                    }
//                    //暂停服务
//                    if (cmd.ToLower() == "stop")
//                    {
//                        svr.Stop();
//                        Console.WriteLine("Server is Stop.");
//                        continue;
//                    }
//                    //开启服务
//                    if (cmd.ToLower() == "start")
//                    {
//                        svr.Start();
//                        Console.WriteLine("Server is listen...{0}", svr.ServerSocket.LocalEndPoint.ToString());
//                        continue;
//                    }
//                    //连接数
//                    if (cmd.ToLower() == "count")
//                    {
//                        Console.WriteLine("Current count of Client is {0}/{1}", svr.SessionCount, svr.Capacity);
//                        continue;
//                    }
//                    //发送数据到客户端格式:send [Session] [stringData]
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

//                    //从服务器上踢掉一个客户端 格式： kick [Session]
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

//                    //列出服务器上所有的客户端信息
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
//        /// 客户端建立连接
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
//        /// 客户端已满
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
//        /// 关闭客户端
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
//        /// 接收数据
//        /// </summary>
//        /// <param name="sender"></param>
//        /// <param name="e"></param>
//        void RecvData(object sender, NetEventArgs e)
//        {
//            //显示接收到的数据
//            Console.WriteLine("Received data: " + e.Client.Datagram + " From: " + e.Client);
//            Console.Write(">");
//        }
//    }
//}
