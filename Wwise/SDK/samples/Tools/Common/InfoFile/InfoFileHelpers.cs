using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;

namespace AK.Wwise.InfoFile
{
    class InfoFileHelpers
    {
        const string INFO_SCHEMA_VERSION = "16";

        // Display any validation errors.
        private static void ValidationCallBack(object sender, ValidationEventArgs e)
        {
            throw new Exception("\n\nInvalid XML format. \n\n(" + e.Message + ")");
        }

        /// <summary>
        /// Load an soundbanks info file. Its data is returned in a SoundBanksInfo class.
        /// </summary>
        /// <param name="file">INFO file to be loaded</param>
        static internal AK.Wwise.InfoFile.SoundBanksInfo LoadInfoFile(string in_file)
        {
            XmlReaderSettings readerSettings = new XmlReaderSettings();
            readerSettings.ValidationType = ValidationType.Schema;
            string szSchemaFolder = System.IO.Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath);
            string szSchemaFile = System.IO.Path.Combine(szSchemaFolder, @"..\..\..\Data\Schemas\SoundbanksInfo.xsd");
            readerSettings.Schemas.Add("", szSchemaFile);
            readerSettings.ValidationType = ValidationType.Schema;
            readerSettings.ValidationEventHandler += new ValidationEventHandler(ValidationCallBack);
            readerSettings.Schemas.Compile();

            //Check the schema version before loading the entire file.
            using (XmlReader reader = XmlReader.Create(in_file, readerSettings))
            {
                // Verify the schema version.
                reader.MoveToContent();
                if (reader.GetAttribute("SchemaVersion") != INFO_SCHEMA_VERSION)
                {
                    throw new Exception("Wrong Info file schema version. Please check the version of Wwise used to generate your SoundBanks.");
                }

                // This is the actual xsd validation for the entire file.
                while (reader.Read()) {}
            }

            // WG-55710 Make sure to deserialize using a .NET stream in order to support XML files greater than 2GB.
            // The Xsd2Code-generated function LoadFromFile loads the XML data entirely in memory before de-serialization! Avoid it at all cost!
            using (var file = new System.IO.FileStream(in_file, System.IO.FileMode.Open, System.IO.FileAccess.Read))
            {
                return AK.Wwise.InfoFile.SoundBanksInfo.Deserialize(file);
            }
        }
    }

    public partial class SoundBanksInfo
    {
        private static XmlSerializer serializer;

        private static XmlSerializer Serializer
        {
            get
            {
                if ((serializer == null))
                {
                    serializer = new XmlSerializerFactory().CreateSerializer(typeof(SoundBanksInfo));
                }
                return serializer;
            }
        }


        #region Serialize/Deserialize
        /// <summary>
        /// Serializes current SoundBanksInfo object into an XML string
        /// </summary>
        /// <returns>string XML value</returns>
        public virtual string Serialize()
        {
            System.IO.StreamReader streamReader = null;
            System.IO.MemoryStream memoryStream = null;
            try
            {
                memoryStream = new System.IO.MemoryStream();
                System.Xml.XmlWriterSettings xmlWriterSettings = new System.Xml.XmlWriterSettings();
                System.Xml.XmlWriter xmlWriter = XmlWriter.Create(memoryStream, xmlWriterSettings);
                Serializer.Serialize(xmlWriter, this);
                memoryStream.Seek(0, System.IO.SeekOrigin.Begin);
                streamReader = new System.IO.StreamReader(memoryStream);
                return streamReader.ReadToEnd();
            }
            finally
            {
                if ((streamReader != null))
                {
                    streamReader.Dispose();
                }
                if ((memoryStream != null))
                {
                    memoryStream.Dispose();
                }
            }
        }

        /// <summary>
        /// Deserializes workflow markup into an SoundBanksInfo object
        /// </summary>
        /// <param name="input">string workflow markup to deserialize</param>
        /// <param name="obj">Output SoundBanksInfo object</param>
        /// <param name="exception">output Exception value if deserialize failed</param>
        /// <returns>true if this Serializer can deserialize the object; otherwise, false</returns>
        public static bool Deserialize(string input, out SoundBanksInfo obj, out System.Exception exception)
        {
            exception = null;
            obj = default(SoundBanksInfo);
            try
            {
                obj = Deserialize(input);
                return true;
            }
            catch (System.Exception ex)
            {
                exception = ex;
                return false;
            }
        }

        public static bool Deserialize(string input, out SoundBanksInfo obj)
        {
            System.Exception exception = null;
            return Deserialize(input, out obj, out exception);
        }

        public static SoundBanksInfo Deserialize(string input)
        {
            System.IO.StringReader stringReader = null;
            try
            {
                stringReader = new System.IO.StringReader(input);
                return ((SoundBanksInfo)(Serializer.Deserialize(XmlReader.Create(stringReader))));
            }
            finally
            {
                if ((stringReader != null))
                {
                    stringReader.Dispose();
                }
            }
        }

        public static SoundBanksInfo Deserialize(System.IO.Stream s)
        {
            return ((SoundBanksInfo)(Serializer.Deserialize(s)));
        }

        #endregion

        /// <summary>
        /// Serializes current SoundBanksInfo object into file
        /// </summary>
        /// <param name="fileName">full path of outupt xml file</param>
        /// <param name="exception">output Exception value if failed</param>
        /// <returns>true if can serialize and save into file; otherwise, false</returns>
        public virtual bool SaveToFile(string fileName, out System.Exception exception)
        {
            exception = null;
            try
            {
                SaveToFile(fileName);
                return true;
            }
            catch (System.Exception e)
            {
                exception = e;
                return false;
            }
        }

        public virtual void SaveToFile(string fileName)
        {
            System.IO.StreamWriter streamWriter = null;
            try
            {
                string xmlString = Serialize();
                System.IO.FileInfo xmlFile = new System.IO.FileInfo(fileName);
                streamWriter = xmlFile.CreateText();
                streamWriter.WriteLine(xmlString);
                streamWriter.Close();
            }
            finally
            {
                if ((streamWriter != null))
                {
                    streamWriter.Dispose();
                }
            }
        }

        /// <summary>
        /// Deserializes xml markup from file into an SoundBanksInfo object
        /// </summary>
        /// <param name="fileName">string xml file to load and deserialize</param>
        /// <param name="obj">Output SoundBanksInfo object</param>
        /// <param name="exception">output Exception value if deserialize failed</param>
        /// <returns>true if this Serializer can deserialize the object; otherwise, false</returns>
        public static bool LoadFromFile(string fileName, out SoundBanksInfo obj, out System.Exception exception)
        {
            exception = null;
            obj = default(SoundBanksInfo);
            try
            {
                obj = LoadFromFile(fileName);
                return true;
            }
            catch (System.Exception ex)
            {
                exception = ex;
                return false;
            }
        }

        public static bool LoadFromFile(string fileName, out SoundBanksInfo obj)
        {
            System.Exception exception = null;
            return LoadFromFile(fileName, out obj, out exception);
        }

        public static SoundBanksInfo LoadFromFile(string fileName)
        {
            System.IO.FileStream file = null;
            System.IO.StreamReader sr = null;
            try
            {
                file = new System.IO.FileStream(fileName, System.IO.FileMode.Open, System.IO.FileAccess.Read);
                sr = new System.IO.StreamReader(file);
                string xmlString = sr.ReadToEnd();
                sr.Close();
                file.Close();
                return Deserialize(xmlString);
            }
            finally
            {
                if ((file != null))
                {
                    file.Dispose();
                }
                if ((sr != null))
                {
                    sr.Dispose();
                }
            }
        }
    }

    public partial class FileDescriptorType : IEquatable<FileDescriptorType>
    {
        public virtual bool Equals(  FileDescriptorType obj )
        { 
            return this.Id == obj.Id && this.Language == obj.Language; 
        }
        public override int GetHashCode()
        {
            return Tuple.Create(Id, Language).GetHashCode();
        }

        public virtual string GetPath()
        {
            return Path;
        }

    }

    public partial class BankDescriptorType : IEquatable<BankDescriptorType>
    {
        public virtual bool Equals(BankDescriptorType obj)
        {
            return this.Id == obj.Id && this.Language == obj.Language;
        }
        public override string GetPath()
        {
            return Path;
        }
    }

    public partial class MediaDescriptorType : IEquatable<MediaDescriptorType>
    {
        public virtual bool Equals(MediaDescriptorType obj)
        {
            return this.Id == obj.Id && this.Language == obj.Language;
        }
        public bool Loose
        {
            get => Location == Location.Loose && (!Streaming || PrefetchSizeSpecified);
        }
        public bool OtherBank
        {
            get => Location == Location.OtherBank;
        }

        public override string GetPath()
        {
            return CachePath ?? Path;
        }
    }
}
