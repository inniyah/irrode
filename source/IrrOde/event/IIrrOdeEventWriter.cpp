  #include <event/IIrrOdeEventWriter.h>

namespace irr {
  namespace ode {
    IIrrOdeEventWriter::IIrrOdeEventWriter() {
      m_pOdeEventManager=CIrrOdeManager::getSharedInstance();
    }

    IIrrOdeEventWriter::~IIrrOdeEventWriter() {
    }

    void IIrrOdeEventWriter::dataChanged() {
      m_pOdeEventManager->objectChanged(this);
    }
  }
}
