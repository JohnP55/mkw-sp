using MarioKartWii;
using Microsoft.AspNetCore.Mvc.ModelBinding;

namespace Models
{
    [BindRequired]
    public class RankingsRequest
    {
        public required Region region { get; set; }
        public required Course course { get; set; }
    }
}