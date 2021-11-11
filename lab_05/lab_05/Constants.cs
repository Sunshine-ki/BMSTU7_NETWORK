using System;

namespace lab_05
{
    public class Constants
    {
        public readonly static string Host = "smtp.gmail.com";
        public readonly static int Port = 587;
        public enum Args { ToEmail = 0, FromEmail = 1, Password = 2, Word = 3, Directory = 4 }
    }
}
