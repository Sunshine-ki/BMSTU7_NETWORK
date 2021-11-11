using System;

namespace lab_05
{
    class Program
    {
        static void Main(string[] args)
        {
            string toEmail = args[Constants.Args.ToEmail.GetHashCode()];
            string fromEmail = args[Constants.Args.FromEmail.GetHashCode()];
            string password = args[Constants.Args.Password.GetHashCode()];
            string word = args[Constants.Args.Word.GetHashCode()];
            string directory = args[Constants.Args.Directory.GetHashCode()];

            Console.WriteLine($"{toEmail} {fromEmail} {password} {word} {directory}");
            
            var msg = Mail.GetMailMessage(toEmail, fromEmail, word, directory);
            Mail.Send(fromEmail, password, msg);
        }
    }
}
