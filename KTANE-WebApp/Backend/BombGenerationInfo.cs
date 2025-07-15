namespace KTANE.Backend;

public class BombGenerationInputInfo
{
    public int Seed;
    public ushort InitialTime;
    public int MaxMistakes;
    public Dictionary<Type, bool> ActiveModules;
}
