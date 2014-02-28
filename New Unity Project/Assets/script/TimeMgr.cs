using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


public class TimeMgr
{
    //毫秒级
    public static UInt64 getTimeStamp()
    {
        return (ulong)(DateTime.Now.ToUniversalTime().Ticks - 621355968000000000) / 10000000;
    }
}

