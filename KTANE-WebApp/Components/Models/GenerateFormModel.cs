using System.ComponentModel.DataAnnotations;
using KTANE.Backend;
using KTANE.Backend.Modules;

namespace KTANE.Components.Models;

public class GenerateBombFormModel
{
    [Range(1, 3500, ErrorMessage = "Der Wert für \"Zeit-Timer\" muss zwischen 1 Sekunde und 3500 Sekunden liegen")]
    public int InitialTime { get; set; } = 1350;

    [Range(1, 3, ErrorMessage = "Der Wert für \"Maximale Versuche\" muss zwischen 1 und 3 liegen")]
    public int MaxMistakes { get; set; } = 3;

    public string Seed = string.Empty;

    [Range(typeof(bool), "false", "true", ErrorMessage = "Eingabe benötigt")]
    public bool BigButtonActive { get; set; } = false;
    [Range(typeof(bool), "false", "true", ErrorMessage = "Eingabe benötigt")]
    public bool WiresActive { get; set; } = false;
    [Range(typeof(bool), "false", "true", ErrorMessage = "Eingabe benötigt")]
    public bool MorseActive { get; set; } = true;
    [Range(typeof(bool), "false", "true", ErrorMessage = "Eingabe benötigt")]
    public bool MemoryActive { get; set; } = true;
    [Range(typeof(bool), "false", "true", ErrorMessage = "Eingabe benötigt")]
    public bool SimonSaysActive { get; set; } = true;
    [Range(typeof(bool), "false", "true", ErrorMessage = "Eingabe benötigt")]
    public bool PasswordActive { get; set; } = false;

    public BombGenerationInputInfo ToStruct()
    {
        return new BombGenerationInputInfo
        {
            Seed = string.IsNullOrWhiteSpace(Seed) ? Guid.NewGuid().GetHashCode() : Seed.GetHashCode(),
            InitialTime = (ushort)InitialTime,
            MaxMistakes = MaxMistakes,
            ActiveModules = new Dictionary<Type, bool>
            {
                {typeof(BigButton), BigButtonActive},
                {typeof(Wires), WiresActive},
                {typeof(Morse), MorseActive},
                {typeof(Memory), MemoryActive},
                {typeof(SimonSays), SimonSaysActive},
                {typeof(Password), PasswordActive},
            }
        };
    }
}
