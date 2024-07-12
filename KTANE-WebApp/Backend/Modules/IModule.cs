namespace KTANE_WebApp.Backend.Module;

public interface IModule
{
    bool IsActive { get; set; }
    bool IsSolved { get; }

    void Reset();
}
