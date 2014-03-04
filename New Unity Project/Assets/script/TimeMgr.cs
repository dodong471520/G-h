using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


public class TimeMgr
{
    //毫秒级
    public static UInt64 getTimeStampMicro()
    {
        return (ulong)(DateTime.Now.ToUniversalTime().Ticks - 621355968000000000) / 10000;
    }
    //秒级
    public static UInt64 getTimeStamp()
    {
        return (UInt64)((DateTime.Now.ToUniversalTime().Ticks - 621355968000000000) / 10000000);
    }
}

