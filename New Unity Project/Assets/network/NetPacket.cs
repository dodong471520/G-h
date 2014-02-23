using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

class NetPacket
{
    public const int NET_PACKET_HEAD_SIZE = 5;
    const byte PROTOCOL_FLAG = 5; // 0101
    public const int MAX_NET_SEED_SIZE = 16;
    public const int MAX_BUFFER_SIZE = CmdPacket.MAX_CMD_PACKET_SIZE;
    const byte FLAG_ENCRYPT = 1;
    const byte FLAG_COMPRESS = 2;
    const byte FLAG_SYSTEM = 4;

    const short SYS_PACKET_SET_SEED   = 1;
    const short SYS_PACKET_KEEP_ALIVE = 2;
    const short SYS_PACKET_ALIVE_ACK  = 3;

    const ushort MAX_NET_PACKET_SEQ = 65000;
    byte m_pktFlag;
    ushort m_pktDataSize;
    ushort m_pktSeq;
    public List<byte> m_buffer=new List<byte>();
    public List<byte> GetBuffer()
    {
        List<byte> ret = new List<byte>();
       ret.Add(m_pktFlag);
       ret.AddRange(BitConverter.GetBytes(m_pktSeq));
       ret.AddRange(BitConverter.GetBytes(m_pktDataSize));
       ret.AddRange(m_buffer);
       return ret;
    }
    public void AddCmdPacket(CmdPacket packet)
    {
	    ushort dataSize =System.Convert.ToUInt16(packet.GetDataSize());
	    m_pktFlag = PROTOCOL_FLAG<<4;
	    m_pktDataSize = dataSize;
        m_buffer.Clear();
        m_buffer.AddRange(packet.GetData());
    }
    public void SetSeq(ushort seq)
    {
        m_pktSeq = seq;
    }
    public void SetSysPacket()
    {
	    m_pktFlag |= FLAG_SYSTEM;
    }
    public bool Compress()
    {
	    /* compress here */
	    m_pktFlag |= FLAG_COMPRESS;
	    return true;
    }
    public bool Uncompress()
    {
        int flag = FLAG_COMPRESS;
        m_pktFlag &= (byte)(~flag);
	    return true;
    }
    public bool IsSysPacket()
    {
        return (m_pktFlag & FLAG_SYSTEM) != 0;
    }
    public bool IsEncrypt()
    {
        return (m_pktFlag & FLAG_ENCRYPT) != 0;
    }
    public bool IsCompress()
    {
        return (m_pktFlag & FLAG_COMPRESS) != 0;
    }
    public void Encrypt(byte[] seed,int len)
    {
	    /* encrypt here */
	    int i,k;
	    for(i=NET_PACKET_HEAD_SIZE,k=0; i<m_pktDataSize;i++,k++)
	    {
		    if(k==len) k=0;
            m_buffer[i] ^= seed[k];
	    }
	    m_pktFlag |= FLAG_ENCRYPT;
    }
    public void Decrypt(byte[] seed, int len)
    {
	    /* decrypt here */
	    int i,k;
	    for(i=NET_PACKET_HEAD_SIZE,k=0; i<m_pktDataSize;i++,k++)
	    {
		    if(k==len) k=0;
		    m_buffer[i]^=seed[k];
	    }
        int flag = FLAG_ENCRYPT;
        m_pktFlag &= (byte)(~flag);
    }
    public int BeginRead(List<byte> readByte)
    {
	    if (readByte.Count < NET_PACKET_HEAD_SIZE) return 0;

	    m_pktFlag =readByte[0];
	    m_pktSeq  = BitConverter.ToUInt16(readByte.ToArray(),1);
	    m_pktDataSize = BitConverter.ToUInt16(readByte.ToArray(),3);
	
	    if (m_pktFlag>>4 != PROTOCOL_FLAG)
	    {
		    Client.Sys_Log("NetPacket Flag Error");
		    return -1;
	    }
	    if (m_pktDataSize <= 0 || m_pktDataSize > CmdPacket.MAX_CMD_PACKET_SIZE) 
	    {
		    Client.Sys_Log("NetPacket Size Error : s = "+m_pktDataSize);
		    return -1;
	    }
	    int total =m_pktDataSize + NET_PACKET_HEAD_SIZE;
	    if (readByte.Count < total) 
	    {
		    // Sys_Log("NetPacket Error 2: len = %d,s = %d",len,s);
		    return 0;
	    }
        m_buffer.Clear();
        m_buffer.AddRange(readByte.GetRange(NET_PACKET_HEAD_SIZE, total - NET_PACKET_HEAD_SIZE));
	    return total;
    }
    public void AttachCmdPacket(CmdPacket packet)
    {
	    packet.BeginRead(m_buffer);
    }
}
