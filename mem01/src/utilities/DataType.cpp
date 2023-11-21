// ================================================================================================
//
// If not explicitly stated: Copyright (C) 2017, all rights reserved,
//      Rüdiger Göbl
//		Email r.goebl@tum.de
//      Chair for Computer Aided Medical Procedures
//      Technische Universität München
//      Boltzmannstr. 3, 85748 Garching b. München, Germany
//
// ================================================================================================

#include "DataType.h"

BEGIN_NAMESPACE_ESI

template <>
DataType DataTypeGet<bool>() {
    return TypeBool;
}
template <>
DataType DataTypeGet<int8_t>() {
    return TypeInt8;
}
template <>
DataType DataTypeGet<uint8_t>() {
    return TypeUint8;
}
template <>
DataType DataTypeGet<int16_t>() {
    return TypeInt16;
}
template <>
DataType DataTypeGet<uint16_t>() {
    return TypeUint16;
}
template <>
DataType DataTypeGet<int32_t>() {
    return TypeInt32;
}
template <>
DataType DataTypeGet<uint32_t>() {
    return TypeUint32;
}
template <>
DataType DataTypeGet<int64_t>() {
    return TypeInt64;
}
template <>
DataType DataTypeGet<uint64_t>() {
    return TypeUint64;
}
#ifdef HAVE_CUDA
template <>
DataType DataTypeGet<__half>() {
    return TypeHalf;
}
#endif
template <>
DataType DataTypeGet<float>() {
    return TypeFloat;
}
template <>
DataType DataTypeGet<double>() {
    return TypeDouble;
}
template <>
DataType DataTypeGet<std::string>() {
    return TypeString;
}

template <>
DataType DataTypeGet<DataType>() {
    return TypeDataType;
}

DataType DataTypeFromString(const std::string &s, bool *success) {
    DataType dataType;
    bool hadSuccess = true;
    if (s == "bool") {
        dataType = TypeBool;
    } else if (s == "int8" || s == "int8_t") {
        dataType = TypeInt8;
    } else if (s == "uint8" || s == "uint8_t") {
        dataType = TypeUint8;
    } else if (s == "int16" || s == "int16_t") {
        dataType = TypeInt16;
    } else if (s == "uint16" || s == "uint16_t") {
        dataType = TypeUint16;
    } else if (s == "int32" || s == "int32_t") {
        dataType = TypeInt32;
    } else if (s == "uint32" || s == "uint32_t") {
        dataType = TypeUint32;
    } else if (s == "int64" || s == "int64_t") {
        dataType = TypeInt64;
    } else if (s == "uint64" || s == "uint64_t") {
        dataType = TypeUint64;
    }
#ifdef HAVE_CUDA
    else if (s == "half") {
        dataType = TypeHalf;
    }
#endif
    else if (s == "float") {
        dataType = TypeFloat;
    } else if (s == "double") {
        dataType = TypeDouble;
    } else if (s == "string") {
        dataType = TypeString;
    } else if (s == "dataType") {
        dataType = TypeDataType;
    } else if (s == "Unknown") {
        dataType = TypeUnknown;
    } else {
        hadSuccess = false;
    }
    if (success) {
        *success = hadSuccess;
    }
    return dataType;
}

std::string DataTypeToString(DataType t, bool *success) {
    std::string s;
    bool hadSuccess = true;
    switch (t) {
        case TypeBool:
            s = "bool";
            break;
        case TypeInt8:
            s = "int8_t";
            break;
        case TypeUint8:
            s = "uint8_t";
            break;
        case TypeInt16:
            s = "int16_t";
            break;
        case TypeUint16:
            s = "uint16_t";
            break;
        case TypeInt32:
            s = "int32_t";
            break;
        case TypeUint32:
            s = "uint32_t";
            break;
        case TypeInt64:
            s = "int64_t";
            break;
        case TypeUint64:
            s = "uint64_t";
            break;
#ifdef HAVE_CUDA
        case TypeHalf:
            s = "half";
            break;
#endif
        case TypeFloat:
            s = "float";
            break;
        case TypeDouble:
            s = "double";
            break;
        case TypeString:
            s = "string";
            break;
        case TypeDataType:
            s = "DataType";
            break;
        case TypeUnknown:
            s = "Unknown";
            break;
        default:
            hadSuccess = false;
            break;
    }

    if (success) {
        *success = hadSuccess;
    }
    return s;
}

std::ostream &operator<<(std::ostream &os, DataType dataType) {
    bool success;
    std::string s = DataTypeToString(dataType, &success);

    if (success) {
        os << s;
    } else {
        os.setstate(std::ios_base::failbit);
    }

    return os;
}

std::istream &operator>>(std::istream &is, DataType &dataType) {
    std::string s;
    is >> s;

    bool success;
    dataType = DataTypeFromString(s, &success);
    if(!success) {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

END_NAMESPACE_ESI
