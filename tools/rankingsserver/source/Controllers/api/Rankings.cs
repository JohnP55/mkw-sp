using Log;
using MKLeaderboards;
using MarioKartWii;
using Microsoft.AspNetCore.Mvc;
using Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Timers;
using log4net;

[ApiController]
[Route("api/")]
public class Rankings : Controller
{
    private static readonly ILog sLog = Logger.GetLogger();
    private static bool sInitialised = false;
    private static readonly Leaderboard[,] sLeaderboards = new Leaderboard[Enum.GetNames(typeof(MKLeaderboards.Region)).Length, Enum.GetNames(typeof(Course)).Length];
    private static readonly List<Task> sTaskList = new List<Task>(Enum.GetNames(typeof(MKLeaderboards.Region)).Length * Enum.GetNames(typeof(Course)).Length);

    public static void Init()
    {
        if (sInitialised)
        {
            return;
        }

        RefreshLeaderboardData();
        StartRefreshTimer();

        sInitialised = true;
    }

    [HttpGet("GetTopRanking")]
    public ActionResult GetTopRanking([FromQuery] RankingsRequest rankingsRequest)
    {
        return HandleRankingsRequest(rankingsRequest, 1);
    }

    [HttpGet("GetTopTenRankings")]
    public ActionResult GetTopTenRankings([FromQuery] RankingsRequest rankingsRequest)
    {
        return HandleRankingsRequest(rankingsRequest, 10);
    }

    private ActionResult HandleRankingsRequest(RankingsRequest rankingsRequest, int recordsRequested)
    {
        string remoteIpAddress;
        
        if (HttpContext.Connection.RemoteIpAddress != null)
        {
            remoteIpAddress = HttpContext.Connection.RemoteIpAddress.ToString();
        }
        else
        {
            remoteIpAddress = "unknown IP address";
        }

        sLog.Info($"Received a HTTP request from {remoteIpAddress}");
        return File(SerializeRankingsData(rankingsRequest, recordsRequested), "application/octet-stream");
    }

    private byte[] SerializeRankingsData(RankingsRequest rankingsRequest, int recordsRequested)
    {
        MKLeaderboards.Region region = MKLeaderboards.Helper.GetMKLeaderboardsRegion(rankingsRequest.region);
        Leaderboard leaderboard = sLeaderboards[(int)region, (int)rankingsRequest.course];
        RankingsResponse rankingsResponse;

        if (leaderboard != null)
        {
            rankingsResponse = new RankingsResponse(leaderboard, recordsRequested);
        }
        else
        {
            rankingsResponse = new RankingsResponse();
        }

        MemoryStream memoryStream = new MemoryStream();
        ProtoBuf.Serializer.Serialize(memoryStream, rankingsResponse);
        return memoryStream.ToArray();
    }

    private static void StartRefreshTimer()
    {
        const int minutesBetweenRefreshes = 60;

        System.Timers.Timer timer = new System.Timers.Timer(TimeSpan.FromMinutes(minutesBetweenRefreshes).TotalMilliseconds);
        timer.Elapsed += (_, _) =>
        {
            RefreshLeaderboardData();
        };
        timer.Enabled = true;
    }

    private static void RefreshLeaderboardData()
    {
        const int millisecondsBetweenRequests = 500;

        foreach (MKLeaderboards.Region region in Enum.GetValues(typeof(MKLeaderboards.Region)))
        {
            foreach (Course course in Enum.GetValues(typeof(Course)))
            {
                sTaskList.Add(Task.Run(async () =>
                {
                    Leaderboard? leaderboard = await MKLeaderboards.Request.GetRankingsData(region, course);
                    string courseName = course.ToString();
                    string regionName = region.ToString();

                    if (leaderboard == null)
                    {
                        sLog.Error($"Failed to update the {courseName} rankings for {regionName}");
                        return;
                    }

                    sLog.Info($"Successfully updated the {courseName} rankings for {regionName}");
                    sLeaderboards[(int)region, (int)course] = leaderboard;
                }));
                Thread.Sleep(millisecondsBetweenRequests);
            }
        }
        Task.WaitAll(sTaskList.ToArray());
        sTaskList.Clear();
    }
}