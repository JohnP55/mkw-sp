using ProtoBuf;
using System;
using System.Diagnostics.CodeAnalysis;
using System.Linq;

namespace Models
{
    [ProtoContract]
    public class Placement
    {
        [ProtoMember(1, IsRequired = true)]
        public required string name { get; set; }

        [ProtoMember(2, IsRequired = true)]
        public required uint time { get; set; }

        [SetsRequiredMembers]
        public Placement(string name, uint time)
        {
            const int maxNameLength = 10;
            const int maxTime = 5999999;

            this.name = name is null ? string.Empty : name.Substring(Math.Min(name.Length, maxNameLength));
            this.time = Math.Min(time, maxTime);
        }
    }

    [ProtoContract]
    public class RankingsResponse
    {
        [ProtoMember(1, IsRequired = true)]
        public required Placement[]? placements { get; set; }

        [SetsRequiredMembers]
        public RankingsResponse()
        {
            placements = null;
        }

        [SetsRequiredMembers]
        public RankingsResponse(Leaderboard leaderboard, int recordsRequested)
        {
            const int maxPlacements = 10;

            if (leaderboard.data.Length > 0 && recordsRequested > 0)
            {
                recordsRequested = new int[]
                {
                    leaderboard.data.Length,
                    recordsRequested,
                    maxPlacements
                }.Min();
                
                placements = new Placement[recordsRequested];
                
                for (int i = 0; i < recordsRequested; i++)
                {
                    placements[i] = new Placement(leaderboard.data[i].name, leaderboard.data[i].score);
                }
            }
            else
            {
                placements = null;
            }
        }
    }
}