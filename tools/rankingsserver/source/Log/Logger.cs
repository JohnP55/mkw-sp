using System.Runtime.CompilerServices;
using log4net;

namespace Log
{
    public class Logger
    {
        public Logger()
        {
            log4net.Config.XmlConfigurator.Configure();
        }

        public static ILog GetLogger([CallerFilePath] string filepath = "")
        {
            int index;

            if (filepath == null)
            {
                filepath = "Logger.cs";
            }
            else if ((index = filepath.LastIndexOf('/')) != -1)
            {
                filepath = filepath.Substring(index + 1);
            }
            else if ((index = filepath.LastIndexOf('\\')) != -1)
            {
                filepath = filepath.Substring(index + 1);
            }

            return LogManager.GetLogger(filepath);
        }
    }
}