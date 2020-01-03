#ifndef CUHdbACTIONFACTORYI_H
#define CUHdbACTIONFACTORYI_H

class CumbiaHdb;

#include <string>
#include <cuhdbactioni.h>

/** \brief Factory to instantiate CuTangoActionI implementations, such as readers, writers and
 *         attribute configuration actions.
 *
 * Notable implementations
 *
 * \li CuTangoReaderFactory
 * \li CuTangoWriterFactory
 * \li CuTangoAttConfFactory
 *
 */
class CuHdbActionFactoryI
{
public:

    virtual ~CuHdbActionFactoryI() {}

    /**
     * Pure virtual method that must be implemented by subclasses to return a valid CuTangoActionI
     * instance.
     *
     * @param source a string identifying a source to connect to
     * @param ct a pointer to a CumbiaHdb instance
     */
    virtual CuHdbActionI *create(const std::string& source, CumbiaHdb *ct) const = 0;

    /** \brief Return the type of action that the factory creates
     *
     * @return the type of action that the factory creates
     */
    virtual CuHdbActionI::Type getType() const = 0;
};

#endif // CUTANGOACTIONFACTORYI_H
