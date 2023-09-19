using Log;
using System;
using log4net;

namespace MKLeaderboards
{
    public static class Helper
    {
        private static readonly ILog sLog = Logger.GetLogger();

        public static Region GetMKLeaderboardsRegion(MarioKartWii.Region region)
        {
            switch (region)
            {
                case MarioKartWii.Region.Japan:
                case MarioKartWii.Region.Taiwan:
                case MarioKartWii.Region.SouthKorea:
                {
                    return Region.Asia;
                }
                case MarioKartWii.Region.Americas:
                {
                    return Region.Americas;
                }
                case MarioKartWii.Region.Europe:
                {
                    return Region.Europe;
                }
                case MarioKartWii.Region.Australasia:
                {
                    return Region.Oceania;
                }
                case MarioKartWii.Region.Worldwide:
                {
                    return Region.World;
                }
            }
            
            string regionName = region.ToString();

            sLog.Fatal($"Could not convert {regionName} to a MKLeaderboards region");
            Environment.Exit(-1);

            // Not reached
            return Region.World;
        }
    }
}