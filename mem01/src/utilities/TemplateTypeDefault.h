// ================================================================================================
//
// If not explicitly stated: Copyright (C) 2016, all rights reserved,
//      Rüdiger Göbl
//		Email r.goebl@tum.de
//      Chair for Computer Aided Medical Procedures
//      Technische Universität München
//      Boltzmannstr. 3, 85748 Garching b. München, Germany
//
// ================================================================================================

#ifndef __TEMPLATETYPEDEFAULT_H__
#define __TEMPLATETYPEDEFAULT_H__

#include "esiglobal.h"

BEGIN_NAMESPACE_ESI

/// A type defined for ease of type handling in the parameter system.
/// The specializations of this type explicitly define the default values
/// for each type.
template <typename ValueType>
class TemplateTypeDefault {
public:
    /// Returns the default value for this type
    static ValueType getDefault();
};

END_NAMESPACE_ESI

#endif //!__TEMPLATETYPEDEFAULT_H__
