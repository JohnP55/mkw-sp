using Log;
using MarioKartWii;
using Models;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using log4net;

namespace MKLeaderboards
{
    public static class Request
    {
        private static readonly ILog sLog = Logger.GetLogger();
        private static readonly HttpClient sHTTPClient = new HttpClient();
        private static readonly Dictionary<Region, string> sRegionDictionary = new Dictionary<Region, string>
        {
            {Region.Asia, "asia"},
            {Region.Americas, "americas"},
            {Region.Europe, "europe"},
            {Region.Oceania, "oceania"},
            {Region.World, "world"},
        };
        private static readonly Dictionary<Course, int> sCourseDictionary = new Dictionary<Course, int>
        {
            {Course.LuigiCircuit, 49},
            {Course.MooMooMeadows, 50},
            {Course.MushroomGorge, 51},
            {Course.ToadsFactory, 52},
            {Course.MarioCircuit, 53},
            {Course.CoconutMall, 54},
            {Course.DKSummit, 55},
            {Course.WarioGoldMine, 56},
            {Course.DaisyCircuit, 57},
            {Course.KoopaCape, 58},
            {Course.MapleTreeway, 59},
            {Course.GrumbleVolcano, 60},
            {Course.DryDryRuins, 61},
            {Course.MoonviewHighway, 62},
            {Course.BowsersCastle, 63},
            {Course.RainbowRoad, 64},
            {Course.GCNPeachBeach, 65},
            {Course.DSYoshiFalls, 66},
            {Course.SNESGhostValley2, 67},
            {Course.N64MarioRaceway, 68},
            {Course.N64SherbetLand, 69},
            {Course.GBAShyGuyBeach, 70},
            {Course.DSDelfinoSquare, 71},
            {Course.GCNWaluigiStadium, 72},
            {Course.DSDesertHills, 73},
            {Course.GBABowserCastle3, 74},
            {Course.N64DKsJungleParkway, 75},
            {Course.GCNMarioCircuit, 76},
            {Course.SNESMarioCircuit3, 77},
            {Course.DSPeachGardens, 78},
            {Course.GCNDKMountain, 79},
            {Course.N64BowsersCastle, 80},
        };
        
        static Request()
        {
            const int httpRequestTimeoutSeconds = 10;

            sHTTPClient.Timeout = TimeSpan.FromSeconds(httpRequestTimeoutSeconds);
        }

        public static async Task<Leaderboard?> GetRankingsData(Region region, Course course)
        {
            string url = MakeURL(region, course);
            string? response = await MakeRequest(url);

            if (response == null)
            {
                return null;
            }

            Leaderboard? leaderboard = ParseResponse(response!);

            if (leaderboard == null)
            {
                sLog.Error($"Failed to deserialize the JSON data from {url}");
                return null;
            }
            
            sLog.Debug($"Successfully deserialized the JSON data from {url}");
            return leaderboard;
        }

        private static async Task<string?> MakeRequest(string url)
        {
            HttpResponseMessage httpResponseMessage;

            sLog.Debug($"Making a HTTP request to {url}");
            
            try
            {
                httpResponseMessage = await sHTTPClient.GetAsync(url);
            }
            catch
            {
                sLog.Error($"Failed to make a HTTP request to {url}");
                return null;
            }
            
            HttpStatusCode httpStatusCode = httpResponseMessage.StatusCode;
            
            if (httpStatusCode != HttpStatusCode.OK)
            {
                sLog.Error($"The HTTP request to {url} returned a {(int)httpStatusCode} status code");
                return null;
            }
            
            return await httpResponseMessage.Content.ReadAsStringAsync();
        }

        private static Leaderboard? ParseResponse(string response)
        {
            try
            {
                return JsonConvert.DeserializeObject<Leaderboard>(response);
            }
            catch
            {
                return null;
            }
        }

        private static string MakeURL(Region region, Course course)
        {
            string regionName = region.ToString();
            string courseName = course.ToString();

            if (!sRegionDictionary.TryGetValue(region, out string? strRegion))
            {
                sLog.Fatal($"The region dictionary does not contain a value for the key {regionName}");
                Environment.Exit(-1);
            }
            if (!sCourseDictionary.TryGetValue(course, out int iCourse))
            {
                sLog.Fatal($"The course dictionary does not contain a value for the key {courseName}");
                Environment.Exit(-1);
            }

            return $"https://www.mkleaderboards.com/api/charts/mkw_nonsc_{strRegion}/{iCourse}";
        }
    }
}