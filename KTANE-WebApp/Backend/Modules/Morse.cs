namespace KTANE_WebApp.Backend.Module;

public class Morse : IModule
{
    public bool IsSolved { get; set; }
    public int MorseIndex { get; private set; }

    public void Generate(Random random)
    {
        MorseIndex = random.Next(16);
    }

    public override string ToString()
    {
        return $"Solved: {IsSolved}, MorseIndex: {MorseIndex}";
    }
}
