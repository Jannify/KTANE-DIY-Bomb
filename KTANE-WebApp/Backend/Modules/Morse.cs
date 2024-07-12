namespace KTANE_WebApp.Backend.Module;

public class Morse : IModule
{
    public bool IsActive { get; set; }
    public bool IsSolved { get; set; }
    public int MorseIndex { get; private set; }

    public void Generate(Random random)
    {
        MorseIndex = random.Next(16);
    }

    public void Reset()
    {
        IsSolved = false;
    }

    private readonly string[] frequencies = ["3505", "3515", "3522", "3532", "3535", "3542", "3545", "3552", "3555", "3565", "3572", "3575", "3582", "3592", "3595", "3600"];

    public override string ToString()
    {
        return $"Solved: {IsSolved}, MorseIndex: {MorseIndex} - {frequencies[MorseIndex]}MHz";
    }
}
