using Newtonsoft.Json;

namespace Models
{
    [JsonObject(ItemRequired = Required.Always)]
    public class Leaderboard
    {
        public required string chart { get; set; }
        public required string track_abbrev { get; set; }
        public required string track_name { get; set; }
        public required uint track_id { get; set; }
        public required uint? world_record { get; set; }
        public required uint world_record_count { get; set; }
        public required Data[] data { get; set; }
    }

    [JsonObject(ItemRequired = Required.AllowNull)]
    public class Data
    {
        public required uint id { get; set; }
        public required uint player_id { get; set; }
        public required uint? user_id { get; set; }
        public required string name { get; set; }
        public required string country { get; set; }
        public required string country_name { get; set; }
        public required uint score { get; set; }
        public required string score_formatted { get; set; }
        public required string /* bool */ proof { get; set; }
        public required string proof_type { get; set; }
        public required string /* bool */ ghost { get; set; }
        public required uint shortcut { get; set; }
        public required uint rank { get; set; }
        public required string wr_diff { get; set; }
        public required string to_next { get; set; }
        public required string date { get; set; }
    }
}