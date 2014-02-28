using System;
using System.Collections.Generic;
using System.Text;
using System.Net;

namespace TcpCSFramework
{
    public class CmdPacket
    {
        public const int MAX_CMD_PACKET_SIZE = 1024 * 16;
        private List<byte> m_WriteData = new List<byte>();
        public void BeginWrite()
        {
            m_WriteData.Clear();
        }
        public void WriteByte(char c)
        {
            m_WriteData.Add((byte)c);
        }
        public void WriteUShort(UInt16 l)
        {
            m_WriteData.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder((short)l)));
        }
        public void WriteInt64(Int64 l)
        {
            m_WriteData.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(l)));
        }
        public void WriteUInt64(UInt64 l)
        {
            m_WriteData.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder((long)l)));
        }
        public void WriteShort(short l)
        {
            m_WriteData.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(l)));
        }
        public void WriteFloat(float l)
        {
            m_WriteData.AddRange(BitConverter.GetBytes(l));
        }
        public void WriteInt(int l)
        {
            m_WriteData.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(l)));
        }
        public void WriteUInt(UInt32 l)
        {
            m_WriteData.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder((int)l)));
        }
        public void WriteString(string l)
        {
            WriteUShort(System.Convert.ToUInt16(System.Text.Encoding.Default.GetByteCount(l)));
            m_WriteData.AddRange(System.Text.Encoding.Default.GetBytes(l));
        }
        private byte[] m_ReadData;
        private int m_ReadOffset = 0;
        public void BeginRead()
        {
            m_ReadData = m_WriteData.ToArray();
            m_ReadOffset = 0;
        }
        public void BeginRead(byte[] bytes)
        {
            m_ReadData = bytes;
            m_ReadOffset = 0;
        }
        public bool ReadByte(out char c)
        {
            c = '\0';
            if (m_ReadOffset + 1 > m_ReadData.Length)
                return false;
            c = (char)m_ReadData[m_ReadOffset];
            m_ReadOffset += 1;
            return true;
        }
        public bool ReadUShort(out UInt16 l)
        {
            l = 0;
            if (m_ReadOffset + 2 > m_ReadData.Length)
                return false;
            l = BitConverter.ToUInt16(m_ReadData, m_ReadOffset);
            l = (UInt16)IPAddress.NetworkToHostOrder((short)l);
            m_ReadOffset += 2;
            return true;
        }
        public bool ReadShort(out short l)
        {
            l = 0;
            if (m_ReadOffset + 2 > m_ReadData.Length)
                return false;
            l = BitConverter.ToInt16(m_ReadData, m_ReadOffset);
            l = IPAddress.NetworkToHostOrder(l);
            m_ReadOffset += 2;
            return true;
        }
        public bool ReadFloat(out float l)
        {
            l = 0;
            if (m_ReadOffset + 4 > m_ReadData.Length)
                return false;
            l = BitConverter.ToSingle(m_ReadData, m_ReadOffset);
            m_ReadOffset += 4;
            return true;
        }
        public bool ReadInt(out int l)
        {
            l = 0;
            if (m_ReadOffset + 4 > m_ReadData.Length)
                return false;
            l = BitConverter.ToInt32(m_ReadData, m_ReadOffset);
            l = IPAddress.NetworkToHostOrder(l);
            m_ReadOffset += 4;
            return true;
        }
        public bool ReadUInt(out UInt32 l)
        {
            l = 0;
            if (m_ReadOffset + 4 > m_ReadData.Length)
                return false;
            int s = BitConverter.ToInt32(m_ReadData, m_ReadOffset);
            l = (UInt32)IPAddress.NetworkToHostOrder(s);
            m_ReadOffset += 4;
            return true;
        }
        public bool ReadInt64(out Int64 l)
        {
            l = 0;
            if (m_ReadOffset + 8 > m_ReadData.Length)
                return false;
            long s = BitConverter.ToInt64(m_ReadData, m_ReadOffset);
            l = IPAddress.NetworkToHostOrder(s);
            m_ReadOffset += 8;
            return true;
        }
        public bool ReadUInt64(out UInt64 l)
        {
            l = 0;
            if (m_ReadOffset + 8 > m_ReadData.Length)
                return false;
            long s = BitConverter.ToInt64(m_ReadData, m_ReadOffset);
            l = (UInt64)IPAddress.NetworkToHostOrder(s);
            m_ReadOffset += 8;
            return true;
        }
        public bool ReadString(out string l)
        {
            l = "";
            ushort length = 0;
            if (!ReadUShort(out length))
                return false;
            if (m_ReadOffset + length > m_ReadData.Length)
                return false;
            l = System.Text.Encoding.Default.GetString(m_ReadData, m_ReadOffset, length);
            m_ReadOffset += length;
            return true;
        }
        public byte[] GetWriteData()
        {
            return m_WriteData.ToArray();
        }
        public byte[] GetReadData()
        {
            return m_ReadData;
        }
        public void BeginRead(List<byte> readByte)
        {
            m_ReadData = readByte.ToArray();
        }
    }
}