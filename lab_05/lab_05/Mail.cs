using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Mail;

namespace lab_05
{
    public class Mail
    {
        public static void Send(string fromEmail, string password, MailMessage message)
        {
            var client = new SmtpClient(Constants.Host, Constants.Port)
            {
                Credentials = new NetworkCredential(fromEmail, password),
                EnableSsl = true
            };

            try
            {
                client.Send(message);
            }
            catch (Exception ex)
            {
                Console.WriteLine("Exception {0}", ex.ToString());
            }

            client.Dispose();
        }

        public static MailMessage GetMailMessage(string toEmail, string fromEmail, string word, string directory)
        {
            MailMessage message = new MailMessage(fromEmail, toEmail, "Files", $"These are the files that contain the word \"{word}\"");

            foreach (var fileName in getFileNames(directory, word))
            {
                message.Attachments.Add(getAttachmentByFileName(fileName));
            }

            return message;
        }

        private static List<string> getFileNames(string directory, string word)
        {
            var result = new List<string>();

            foreach (var file in Directory.GetFiles(directory))
            {
                if (File.ReadAllText(file).Contains(word))
                {
                    result.Add(file);
                }
            }

            return result;
        }

        private static Attachment getAttachmentByFileName(string fileName)
        {
            Attachment data = new Attachment(fileName, System.Net.Mime.MediaTypeNames.Application.Octet);

            // Add time stamp information for the file.
            System.Net.Mime.ContentDisposition disposition = data.ContentDisposition;
            disposition.CreationDate = File.GetCreationTime(fileName);
            disposition.ModificationDate = File.GetLastWriteTime(fileName);
            disposition.ReadDate = File.GetLastAccessTime(fileName);

            return data;
        }
    }
}
