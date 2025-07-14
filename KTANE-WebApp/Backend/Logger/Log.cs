using System.Diagnostics;
using Serilog;
using Serilog.Sinks.SystemConsole.Themes;

namespace KTANE.Backend.Logger;

public static class Log
{
    private static Serilog.ILogger logger = Serilog.Core.Logger.None;
    private static bool isSetup;


    public static void Setup()
    {
        if (isSetup)
        {
            Log.Warn($"{nameof(Log)} setup should only be executed once.");
            return;
        }

        isSetup = true;

        logger = new LoggerConfiguration()
            .MinimumLevel.Debug()
            .WriteTo.Logger(cnf =>
            {
                string consoleTemplate = $"[{{Timestamp:HH:mm:ss.fff}}] [{{Level:u3}}] {{Message}}{{NewLine}}{{Exception}}";

                cnf.WriteTo.Console(outputTemplate: consoleTemplate, theme: AnsiConsoleTheme.Code);
            })
            .WriteTo.Logger(cnf =>
            {
                cnf.MinimumLevel.Information();
                cnf.WriteTo.WebsiteSink();
            })
            .CreateLogger();
    }

    [Conditional("DEBUG")]
    public static void Debug(string message)
    {
        logger.Debug(message);
    }

    public static void Info(string message)
    {
        logger.Information(message);
    }

    public static void Warn(string message)
    {
        logger.Warning(message);
    }

    public static void Error(string message)
    {
        logger.Error(message);
    }

    public static void Error(Exception ex)
    {
        logger.Error(ex, ex.Message);
    }

    public static void Error(Exception ex, string message)
    {
        logger.Error(ex, message);
    }
}
