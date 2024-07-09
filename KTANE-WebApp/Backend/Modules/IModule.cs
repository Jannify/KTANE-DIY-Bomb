namespace KTANE_WebApp.Backend.Module;

public interface IModule
{
    bool IsSolved { get; }

    void Reset();
}
