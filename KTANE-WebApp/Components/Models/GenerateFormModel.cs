using System.ComponentModel.DataAnnotations;
using KTANE_WebApp.Backend;

namespace KTANE_WebApp.Components.Models;

public class GenerateBombFormModel
{
    [Range(1, 3500, ErrorMessage = "Der Wert für \"Zeit-Timer\" muss zwischen 1 Sekunde und 3500 Sekunden liegen")]
    public int InitialTime { get; set; } = 150;

    [Range(1, 3, ErrorMessage = "Der Wert für \"Maximale Versuche\" muss zwischen 1 und 3 liegen")]
    public int MaxMistakes { get; set; } = 3;

    public string Seed = string.Empty;

    public BombGenerationInputInfo ToStruct()
    {
        return new BombGenerationInputInfo
        {
            Seed = string.IsNullOrWhiteSpace(Seed) ? Guid.NewGuid().GetHashCode() : Seed.GetHashCode(),
            InitialTime = (ushort)InitialTime,
            MaxMistakes = MaxMistakes
        };
    }
}
