#ifndef _C_CONFIG_FILE_MANAGER
  #define _C_CONFIG_FILE_MANAGER

  #include <irrlicht.h>

using namespace irr;

/**
 * This interface must be implemented by all classes that read from the global config file
 * @author Christian Keimel / dustbin::games
 */
class IConfigFileReader {
  public:
	  /**
		 * Implement this method to read from the global config file
		 */
    virtual void readConfig(io::IXMLReaderUTF8 *pXml)=0;
};

/**
 * This interface must be implemented by all classes that write to the global config file
 * @author Christian Keimel / dustbin::games
 */
class IConfigFileWriter {
  public:
	  /**
		 * Implement this method to write your section to the global config file
		 */
    virtual void writeConfig(io::IXMLWriter *pXml)=0;
};

/**
 * The CConfigFileManager manages reading and writing of the global configuration file
 * @author Christian Keimel / dustbin::games
 */
class CConfigFileManager {
  protected:
    core::list<IConfigFileReader *> m_lReaders;		/**< list of all config file readers */
    core::list<IConfigFileWriter *> m_lWriters;		/**< list of all config file writers */

  public:
	  /**
		 * Get the singleton instance of the manager
		 * @return the shared instance of the manager
		 */
    static CConfigFileManager *getSharedInstance();

		/**
		 * add a config file reader
		 * @param the reader to be added
		 */
    void addReader(IConfigFileReader *pReader);

		/**
		 * add a config file writer
		 * @param the writer to be added
		 */
    void addWriter(IConfigFileWriter *pWriter);

		/**
		 * remove a config file reader
		 * @param the reader to be removed
		 */
    void removeReader(IConfigFileReader *pReader);

		/**
		 * remove a config file writer
		 * @param the writer to be removed
		 */
    void removeWriter(IConfigFileWriter *pWriter);

		/**
		 * clear the list of all readers and writers
		 */
    void clearReadersWriters();

		/**
		 * load a configuration file
		 * @param pDevice the Irrlicht device to use
		 * @param sFileName the name of the config file to load
		 */
    void loadConfig(IrrlichtDevice *pDevice, const c8 *sFileName);

		/**
		 * write a configuration file
		 * @param pDevice the Irrlicht device to use
		 * @param sFileName the name of the config file to write
		 */
    void writeConfig(IrrlichtDevice *pDevice, const c8 *sFileName);
};

#endif
