using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
namespace Network
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
            m_WriteData.AddRange(BitConverter.GetBytes(c));
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
        public void WriteUInt(uint l)
        {
            int m = (int)l;
            m_WriteData.AddRange(BitConverter.GetBytes(IPAddress.HostToNetworkOrder(m)));
        }
        public void WriteString(string l)
        {
            WriteShort(System.Convert.ToInt16(System.Text.Encoding.Default.GetByteCount(l)+1));
            m_WriteData.AddRange(System.Text.Encoding.Default.GetBytes(l));
            m_WriteData.Add(0);
        }
        private byte[] m_ReadData;
        private int m_ReadOffset = 0;
        public void BeginRead()
        {
            m_ReadData = m_WriteData.ToArray();
            m_ReadOffset = 0;
        }
        public bool ReadByte(ref char c)
        {
            if (m_ReadOffset + 1 > m_ReadData.Length)
                return false;
            c = BitConverter.ToChar(m_ReadData, m_ReadOffset);
            m_ReadOffset += 1;
            return true;
        }
        public bool ReadShort(ref short l)
        {
            if (m_ReadOffset + 2 > m_ReadData.Length)
                return false;
            l = IPAddress.NetworkToHostOrder(BitConverter.ToInt16(m_ReadData, m_ReadOffset));
            m_ReadOffset += 2;
            return true;
        }
        public bool ReadFloat(ref float l)
        {
            if (m_ReadOffset + 4 > m_ReadData.Length)
                return false;
            l = BitConverter.ToSingle(m_ReadData, m_ReadOffset);
            m_ReadOffset += 4;
            return true;
        }
        public bool ReadInt(ref int l)
        {
            if (m_ReadOffset + 4 > m_ReadData.Length)
                return false;
            l =IPAddress.NetworkToHostOrder( BitConverter.ToInt32(m_ReadData, m_ReadOffset));
            m_ReadOffset += 4;
            return true;
        }
        public bool ReadUInt(ref uint l)
        {
            if (m_ReadOffset + 4 > m_ReadData.Length)
                return false;
            int mm=IPAddress.NetworkToHostOrder(BitConverter.ToInt32(m_ReadData, m_ReadOffset));
            l = (uint)mm;
            m_ReadOffset += 4;
            return true;
        }
        public bool ReadString(ref string l)
        {
            short length = 0;
            if (!ReadShort(ref length))
                return false;
            if (m_ReadOffset + length > m_ReadData.Length)
                return false;
            m_ReadData[m_ReadOffset + length - 1] = 0;
            l = System.Text.Encoding.Default.GetString(m_ReadData, m_ReadOffset, length);
            m_ReadOffset += length;
            return true;
        }
        public int GetDataSize()
        {
            return m_WriteData.Count;
        }
        public byte[] GetData()
        {
            return m_WriteData.ToArray();
        }
        public void BeginRead(List<byte> readByte)
        {
            m_ReadOffset = 0;
            m_ReadData = readByte.ToArray();
        }
    }
}