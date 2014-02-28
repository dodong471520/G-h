using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
class Proto
{
    public const UInt16 C_AutoMatch = 0x0001;
    public const UInt16 C_UAutoMatch = 0x0002;
    public const UInt16 S_GameInit = 0x0003;
    public const UInt16 C_GameReady = 0x0004;
    public const UInt16 S_GameStart = 0x0005;
    public const UInt16 C_GameShot = 0x0006;
    public const UInt16 S_GameShot = 0x0007;
    public const UInt16 Keep_Alive = 0x0008;
    public const UInt16 Keep_Alive_Ack = 0x0009;
    public const UInt16 Synch_Pos = 0x000a;
}
