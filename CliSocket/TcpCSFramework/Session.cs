using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Diagnostics;

namespace TcpCSFramework
{
    /// <summary>
    /// �ͻ����������֮��ĻỰ��
    /// </summary>
    public class Session : ICloneable
    {
        public void addBuff(byte[] buff)
        {
            if (buff == null || buff.Length == 0)
                return;
            RecvBuffer.AddRange(buff);
            if (RecvBuffer.Count <= 2)
                return ;
            ushort len = (ushort)System.Net.IPAddress.HostToNetworkOrder((short)BitConverter.ToUInt16(RecvBuffer.ToArray(), 0));
            int total = len + 2;
            if (RecvBuffer.Count < total)
                return ;
            RecvPacket = new byte[len];
            RecvBuffer.CopyTo(2, RecvPacket, 0, len);
            RecvBuffer.RemoveRange(0, total);
        }
        #region �ֶ�

        /// <summary>
        /// �ỰID
        /// </summary>
        private SessionId _id;
        /// <summary>
        /// �������ݻ�������С64K
        /// </summary>
        public const int DefaultBufferSize = 1024;
        public byte[] RecvDataBuffer=new byte[DefaultBufferSize];
        private List<byte> RecvBuffer = new List<byte>();
        public byte[] RecvPacket=null;
        /// <summary>
        /// �ͻ��˷��͵��������ı���
        /// ע��:����Щ����±��Ŀ���ֻ�Ǳ��ĵ�Ƭ�϶�������
        /// </summary>
        private string _datagram;

        /// <summary>
        /// �ͻ��˵�Socket
        /// </summary>
        private Socket _cliSock;

        /// <summary>
        /// �ͻ��˵��˳�����
        /// </summary>
        private ExitType _exitType;

        /// <summary>
        /// �Ựͨ����
        /// </summary>
        private string _channel = "";

        /// <summary>
        /// �˳�����ö��
        /// </summary>
        public enum ExitType
        {
            NormalExit,
            ExceptionExit
        };

        #endregion

        #region ����

        /// <summary>
        /// ���ػỰ��ID
        /// </summary>
        public SessionId ID
        {
            get
            {
                return _id;
            }
            set
            {
                _id = value;
            }
        }
        /// <summary>
        /// ��ȡ�Ự�ı���
        /// </summary>
        public string Datagram
        {
            get
            {
                return _datagram;
            }
            set
            {
                _datagram = value;
            }
        }

        /// <summary>
        /// �����ͻ��˻Ự������Socket����
        /// </summary>
        public Socket ClientSocket
        {
            get
            {
                return _cliSock;
                
            }
        }


        /// <summary>
        /// ��ȡ�ͻ��˵��˳���ʽ
        /// </summary>
        public ExitType TypeOfExit
        {
            get
            {
                return _exitType;
            }

            set
            {
                _exitType = value;
            }
        }

        /// <summary>
        /// �Ựִ�е�ͨ����
        /// </summary>
        public string Channel
        {
            get
            {
                return _channel;
            }

            set
            {
                _channel = value;
            }
        }
        
        #endregion

        #region ����

        /// <summary>
        /// ʹ��Socket�����Handleֵ��ΪHashCode,���������õ���������.
        /// </summary>
        /// <returns></returns>
        public override int GetHashCode()
        {
            return (int)_cliSock.Handle;
        }

        /// <summary>
        /// ��������Session�Ƿ����ͬһ���ͻ���
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public override bool Equals(object obj)
        {
            Session rightObj = (Session)obj;

            return (int)_cliSock.Handle == (int)rightObj.ClientSocket.Handle;
        }

        /// <summary>
        /// ����ToString()����,����Session���������
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            string result = string.Format("Session:{0},IP:{1}",
             _id, _cliSock.RemoteEndPoint.ToString());

            //result.C
            return result;
        }

        /// <summary>
        /// ���캯��
        /// </summary>
        /// <param name="cliSock">�Ựʹ�õ�Socket����</param>
        public Session(Socket cliSock)
        {
            Debug.Assert(cliSock != null);

            _cliSock = cliSock;

            _id = new SessionId((int)cliSock.Handle);
        }

        /// <summary>
        /// �رջỰ
        /// </summary>
        public void Close()
        {
            Debug.Assert(_cliSock != null);

            //�ر����ݵĽ��ܺͷ���
            _cliSock.Shutdown(SocketShutdown.Both);

            //������Դ
            _cliSock.Close();
        }

        #endregion

        #region ICloneable ��Ա

        object System.ICloneable.Clone()
        {
            Session newSession = new Session(_cliSock);
            newSession.Datagram = _datagram;
            newSession.TypeOfExit = _exitType;
            return newSession;
        }

        #endregion
    }
}
