#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <QTextEdit>
#include <QTextStream>
#include <QDateTime>
#include <QFile>
#include <QHash>

class TBinaryType;
class TPrimitiveType;
class TArraySinglePrimitive;
class TFileRecord;
class TBinaryInput;
class TBinaryObject;

#define ARRAY_SINGLE 0
#define ARRAY_JAGGED 1
#define ARRAY_RECTANGULAR 2
#define ARRAY_SINGLEOFFSET 3
#define ARRAY_JAGGEDOFFSET 4
#define ARRAY_RECTANGULAROFFSET 5

#define MESSAGE_NOARGS 0x1
#define MESSAGE_ARGSINLINE 0x2
#define MESSAGE_ARGSISARRAY 0x4
#define MESSAGE_ARGSINARRAY 0x8
#define MESSAGE_NOCONTEXT 0x10
#define MESSAGE_CONTEXTINLINE 0x20
#define MESSAGE_CONTEXTINARRAY 0x40
#define MESSAGE_METHODSIGNATUREINARRAY 0x80
#define MESSAGE_PROPERTIESINARRAY 0x100
#define MESSAGE_NORETURNVALUE 0x200
#define MESSAGE_RETURNVALUEVOID 0x400
#define MESSAGE_RETURNVALUEINLINE 0x800
#define MESSAGE_RETURNVALUEINARRAY 0x1000
#define MESSAGE_EXCEPTIONINARRAY 0x2000
#define MESSAGE_GENERICMETHOD 0x8000

#define DESERIAL_OK                 0
#define DESERIAL_FILE_OPEN_FAILED   1
#define DESERIAL_FILE_READ_FAILED   2
#define DESERIAL_NO_HEADER          3
#define DESERIAL_UNKNOWN_RECORD     4
#define DESERIAL_RECORD_READ_FAILED 5

#define MAX_INDENT 20
#define MAX_ARRAY_PRINT_SIZE 20

// ==========================================
// ClassInfo
// ==========================================

class TClassInfo
{
public:
    qint32 objectID;
    QString name;
    QString displayName;
    QList<QString> memberNames;

    bool read(TBinaryInput& input);
    void write(QTextStream& outstr, int indent);
};

// ==========================================
// PrimitiveTypeEnumeration
// ==========================================

class TPrimitiveType
{
public:
    virtual bool read(TBinaryInput&){ return true;}
    virtual void write(QTextStream&) {}
    virtual void writeType(QTextStream&) {}
    virtual TPrimitiveType* clone() { return new TPrimitiveType(*this); }
    virtual bool getBool(bool&) { return false; }
    virtual bool getInt(qint64&) { return false; }
    virtual bool getUnsigned(quint64&) { return false; }
    virtual bool getDouble(double&) { return false; }
    virtual bool getFloat(float&) { return false; }
    virtual bool getString(QString&) { return false; }
    virtual bool getDateTime(QDateTime&) { return false; }

    virtual bool getBoolArray(bool**, int&) { return false; }
    virtual bool getInt32Array(qint32**, int&) { return false; }
    virtual bool getDoubleArray(double**, int&) { return false; }
    virtual bool getObjectArray(TBinaryObject***, int&)  { return false; }
};

// --------- 1 ------------
class TPrimitiveBoolean : public TPrimitiveType
{
public:
    // in file as 8 bit

    bool value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveBoolean* clone() override { return new TPrimitiveBoolean(*this); }
    bool getBool(bool& result) override { result = value; return true; }
};

// --------- 2 ------------
class TPrimitiveByte : public TPrimitiveType
{
public:
    char value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveByte* clone() { return new TPrimitiveByte(*this); }
    bool getInt(qint64& result) override { result = value; return true; }
};

// --------- 3 ------------
class TPrimitiveChar : public TPrimitiveType
{
public:
    // unicode char
    // in file as utf-8

    QString value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveChar* clone() { return new TPrimitiveChar(*this); }
    bool getString(QString& result) override { result = value; return true; }
};

// --------- 5 ------------
class TPrimitiveDecimal : public TPrimitiveType
{
public:
    // in file as string

    double value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveDecimal* clone() { return new TPrimitiveDecimal(*this); }
    bool getDouble(double& result) override { result = value; return true; }
};

// --------- 6 ------------
class TPrimitiveDouble : public TPrimitiveType
{
public:
    // in file as 64 bit

    double value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveDouble* clone() { return new TPrimitiveDouble(*this); }
    bool getDouble(double& result) override { result = value; return true; }
};

// --------- 7 ------------
class TPrimitiveInt16 : public TPrimitiveType
{
public:
    qint16 value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveInt16* clone() { return new TPrimitiveInt16(*this); }
    bool getInt(qint64& result) override { result = value; return true; }
};

// --------- 8 ------------
class TPrimitiveInt32 : public TPrimitiveType
{
public:
    qint32 value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveInt32* clone() { return new TPrimitiveInt32(*this); }
    bool getInt(qint64& result) override { result = value; return true; }
};

// --------- 9 ------------
class TPrimitiveInt64 : public TPrimitiveType
{
public:
    qint64 value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveInt64* clone() { return new TPrimitiveInt64(*this); }
    bool getInt(qint64& result) override { result = value; return true; }
 };

// --------- 10 ------------
class TPrimitiveSByte : public TPrimitiveType
{
public:
    char value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveSByte* clone() { return new TPrimitiveSByte(*this); }
    bool getInt(qint64& result) override { result = value; return true; }
};

// --------- 11 ------------
class TPrimitiveSingle : public TPrimitiveType
{
public:
    // in file as 32 bit float

    float value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveSingle* clone() { return new TPrimitiveSingle(*this); }
    bool getFloat(float& result) override { result = value; return true; }
};

// --------- 12 ------------
class TPrimitiveTimeSpan : public TPrimitiveType
{
public:
    // in file as 64 bit int
    // specifies duration as int number of 100 nanoseconds

    qint64 value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveTimeSpan* clone() { return new TPrimitiveTimeSpan(*this); }
    bool getInt(qint64& result) override { result = value; return true; }
};

// --------- 13 ------------
class TPrimitiveDateTime : public TPrimitiveType
{
public:
    // in file as 62 bit int
    // 2 bit time zone: 0 none, 1 utc, 2 local
    // specifies duration as int number of 100 nanoseconds since jan 0001

    QDateTime datetime;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveDateTime* clone() { return new TPrimitiveDateTime(*this); }
    bool getDateTime(QDateTime& result) { result = datetime; return true; }
};

// --------- 14 ------------
class TPrimitiveUInt16 : public TPrimitiveType
{
public:
    quint16 value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveUInt16* clone() { return new TPrimitiveUInt16(*this); }
    bool getUnsigned(quint64& result) override { result = value; return true; }
};

// --------- 15 ------------
class TPrimitiveUInt32 : public TPrimitiveType
{
public:
    quint32 value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveUInt32* clone() { return new TPrimitiveUInt32(*this); }
    bool getUnsigned(quint64& result) override { result = value; return true; }
};

// --------- 16 ------------
class TPrimitiveUInt64 : public TPrimitiveType
{
public:
    quint64 value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveUInt64* clone() { return new TPrimitiveUInt64(*this); }
    bool getUnsigned(quint64& result) override { result = value; return true; }
};

// --------- 17 ------------
class TPrimitiveNull : public TPrimitiveType
{
public:

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveNull* clone() { return new TPrimitiveNull(*this); }
};

// --------- 18 ------------
class TPrimitiveString : public TPrimitiveType
{
public:
    QString value;

    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TPrimitiveString* clone() { return new TPrimitiveString(*this); }
    bool getString(QString& result) override { result = value; return true; }
};

// ==========================================
// SearchType
// ==========================================

class TSearchType
{
public:
    virtual TBinaryType* getMember(int&, QStringList&) { return NULL; }

    virtual bool getBool(bool&) { return false; }
    virtual bool getInt(qint64&) { return false; }
    virtual bool getUnsigned(quint64&) { return false; }
    virtual bool getDouble(double&) { return false; }
    virtual bool getFloat(float&) { return false; }
    virtual bool getString(QString&) { return false; }
    virtual bool getDateTime(QDateTime&) { return false; }

    virtual bool getBoolArray(bool**, int&) { return false; }
    virtual bool getInt32Array(qint32**, int&) { return false; }
    virtual bool getDoubleArray(double**, int&) { return false; }
    virtual bool getStringArray(QStringList&) { return false; }
    virtual bool getObjectArray(TBinaryObject***, int&)  { return false; }

    virtual void write(QTextStream&, int) {}
};

// ==========================================
// BinaryTypeEnumeration
// ==========================================

class TBinaryType : public TSearchType
{
public:
    TFileRecord* refRecord;
    qint32 refID;

    TBinaryType();
    ~TBinaryType();
    virtual bool read(TBinaryInput&, QList<TFileRecord*>&){ return true;}
    virtual bool readAdditional(TBinaryInput&){ return true;}
    virtual void writeType(QTextStream&) {}
    virtual TBinaryType* cloneType() {return NULL;}

    TBinaryType* getMember(int& index, QStringList& arglist) override;

    bool getBoolArray(bool**, int&) override;
    bool getInt32Array(qint32**, int&) override;
    bool getDoubleArray(double**, int&) override;
    bool getStringArray(QStringList&) override;
    bool getObjectArray(TBinaryObject***, int&);

    bool getBool(bool& result) override;
    bool getInt(qint64& result) override;
    bool getUnsigned(quint64& result) override;
    bool getDouble(double& result) override;
    bool getFloat(float& result) override;
    bool getString(QString& result) override;
    bool getDateTime(QDateTime& result) override;
};

// --------- 0 ------------
class TBinaryPrimitive : public TBinaryType
{
public:
    char typeEnum;
    TPrimitiveType* value;

    TBinaryPrimitive();
    ~TBinaryPrimitive();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    bool readAdditional(TBinaryInput& input) override;
    void write(QTextStream& outstr, int indent) override;
    void writeType(QTextStream& outstr) override;
    TBinaryType* cloneType() override;

    bool getBool(bool& result) override;
    bool getInt(qint64& result) override;
    bool getUnsigned(quint64& result) override;
    bool getDouble(double& result) override;
    bool getFloat(float& result) override;
    bool getString(QString& result) override;
    bool getDateTime(QDateTime& result) override;
};

// --------- 1 ------------
class TBinaryString : public TBinaryType
{
public:
    qint32 objectID;
    QString value;

    TBinaryString();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    bool readNoRef(char ref, TBinaryInput& input, QList<TFileRecord*>& recordList);
    void write(QTextStream& outstr, int indent) override;
    void writeType(QTextStream& outstr) override;
    TBinaryType* cloneType() override;
    bool getString(QString& result) override;
};

// --------- 2 ------------
class TBinaryObject : public TBinaryType
{
public:
    TBinaryObject();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    bool readNoRef(char ref, TBinaryInput& input, QList<TFileRecord*>& recordList);
    void write(QTextStream& outstr, int indent) override;
    void writeType(QTextStream& outstr) override;
    TBinaryType* cloneType() override;
};

// --------- 3 ------------
class TBinarySystemClass : public TBinaryType
{
public:
    QString name;

    TBinarySystemClass();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    bool readAdditional(TBinaryInput& input) override;
    void write(QTextStream& outstr, int indent) override;
    void writeType(QTextStream& outstr) override;
    TBinaryType* cloneType() override;
};

// --------- 4 ------------
class TBinaryClass : public TBinaryType
{
public:
    QString name;
    qint32 libraryID;

    TBinaryClass();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    bool readAdditional(TBinaryInput& input) override;
    void write(QTextStream& outstr, int indent) override;
    void writeType(QTextStream& outstr) override;
    TBinaryType* cloneType() override;
};

// --------- 5 ------------
class TBinaryObjectArray : public TBinaryType
{
public:
    TBinaryObjectArray();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
    void writeType(QTextStream& outstr) override;
    TBinaryType* cloneType() override;
};

// --------- 6 ------------
class TBinaryStringArray : public TBinaryType
{
public:
    TBinaryStringArray();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
    void writeType(QTextStream& outstr) override;
    TBinaryType* cloneType() override;
};

// --------- 7 ------------
class TBinaryPrimitiveArray : public TBinaryType
{
public:
    char typeEnum;

    TBinaryPrimitiveArray();
    ~TBinaryPrimitiveArray();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    bool readAdditional(TBinaryInput& input) override;
    void write(QTextStream& outstr, int indent) override;
    void writeType(QTextStream& outstr) override;
    TBinaryType* cloneType() override;
};

// ==========================================
// RecordTypeEnumeration
// ==========================================

class TFileRecord : public TSearchType
{
public:
    bool isReferenced;
    qint32 objectID;
    TClassInfo classInfo;
    QList<TBinaryType*> memberList;

    TFileRecord();
    ~TFileRecord();
    virtual bool read(TBinaryInput&, QList<TFileRecord*>&){ return true; }
    virtual void getReferences(QList<TBinaryType*>& list);

    TBinaryType* getMember(int& index, QStringList& arglist) override;
};

// --------- 0 ------------
class TSerializedStreamHeader : public TFileRecord
{
public:
    qint32 rootID;
    qint32 headerID;
    qint32 majorVersion;
    qint32 minorVersion;

    TSerializedStreamHeader();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
};

// --------- 1 ------------
class TClassWithId : public TFileRecord
{
public:
    qint32 metadataID;
    TFileRecord* metadataRecord;

    TClassWithId();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
};

// --------- 2 ------------
class TSystemClassWithMembers : public TFileRecord
{
public:

    TSystemClassWithMembers();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
};

// --------- 3 ------------
class TClassWithMembers : public TFileRecord
{
public:
    qint32 libraryID;

    TClassWithMembers();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
};

// --------- 4 ------------
class TSystemClassWithMembersAndTypes : public TFileRecord
{
public:
    qint32 libraryID;

    TSystemClassWithMembersAndTypes();
    ~TSystemClassWithMembersAndTypes();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
};

// --------- 5 ------------
class TClassWithMembersAndTypes : public TFileRecord
{
public:
    qint32 libraryID;

    TClassWithMembersAndTypes();
    ~TClassWithMembersAndTypes();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
};

// --------- 6 ------------
class TBinaryObjectString : public TFileRecord
{
public:
    QString value;

    TBinaryObjectString();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
    bool getString(QString& str);
};

// --------- 7 ------------
class TBinaryArray : public TFileRecord
{
public:
    char arrayType;
    qint32 rank;
    qint32* lengths;
    qint32* lowerBounds;
    TBinaryType* remoteType;
    TBinaryObject** array;
    int arraySize;
    int* offset;

    TBinaryArray();
    ~TBinaryArray();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
    void getReferences(QList<TBinaryType*>& btlist) override;
    TBinaryType* getMember(int& index, QStringList& arglist) override;
};

// --------- 8 ------------
class TMemberPrimitiveTyped : public TFileRecord
{
public:
    TPrimitiveType* value;

    TMemberPrimitiveTyped();
    ~TMemberPrimitiveTyped();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList);
    void write(QTextStream& outstr, int indent) override;
};

// --------- 9 ------------
class TMemberReference : public TFileRecord
{
public:

    TMemberReference();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
    void getReferences(QList<TBinaryType*>& btlist) override;
};

// --------- 10 ------------
class TObjectNull : public TFileRecord
{
public:

    TObjectNull();
    bool read(TBinaryInput&, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
};

// --------- 11 ------------
class TMessageEnd : public TFileRecord
{
public:

    TMessageEnd();
    bool read(TBinaryInput&, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
};

// --------- 12 ------------
class TBinaryLibrary : public TFileRecord
{
public:
    qint32 libraryID;
    QString libraryName;

    TBinaryLibrary();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList);
    void write(QTextStream& outstr, int indent) override;
};

// --------- 13 ------------
class TObjectNullMultiple256 : public TFileRecord
{
public:
    int nullCount;

    TObjectNullMultiple256();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
};

// --------- 14 ------------
class TObjectNullMultiple : public TFileRecord
{
public:
    qint32 nullCount;

    TObjectNullMultiple();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
};

// --------- 15 ------------
class TArraySinglePrimitive : public TFileRecord
{
public:
    qint32 length;
    char type;
    TPrimitiveType* array;

    TArraySinglePrimitive();
    ~TArraySinglePrimitive();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;

    bool getBoolArray(bool**, int&) override;
    bool getInt32Array(qint32**, int&) override;
    bool getDoubleArray(double**, int&) override;
    bool getObjectArray(TBinaryObject***, int&);
};

// --------- 16 ------------
class TArraySingleObject : public TFileRecord
{
public:
    qint32 length;
    TBinaryObject** objectArray;
    char* typeArray;
    double* doubleArray;
    qint32* int32Array;
    qint64* int64Array;
    bool* boolArray;

    TArraySingleObject();
    ~TArraySingleObject();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
    void getReferences(QList<TBinaryType*>& btlist) override;
    TBinaryType* getMember(int &index, QStringList &arglist) override;

    bool getBoolArray(bool** array, int& len) override;
    bool getInt32Array(qint32** array, int& len) override;
    bool getDoubleArray(double** array, int& len) override;
    bool getObjectArray(TBinaryObject*** array, int& len) override;
};

// --------- 17 ------------
class TArraySingleString : public TFileRecord
{
public:
    TArraySingleString();
    ~TArraySingleString();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
    bool getStringArray(QStringList&) override;
};

// --------- 21 ------------
class TMethodCall : public TFileRecord
{
public:
    qint32 messageFlags;
    QString methodName;
    QString typeName;
    QString callContext;
    QList<TPrimitiveType*> args;

    TMethodCall();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
};

// --------- 22 ------------
class TMethodReturn : public TFileRecord
{
public:
    qint32 messageFlags;
    QString methodName;
    QString typeName;
    QString callContext;
    QList<TPrimitiveType*> args;

    TMethodReturn();
    bool read(TBinaryInput& input, QList<TFileRecord*>& recordList) override;
    void write(QTextStream& outstr, int indent) override;
};

// ==========================================
// ArrayTypeEnumeration - array of primitives
// ==========================================

// --------- 1 ------------
class TArrayBoolean : public TPrimitiveType
{
public:
    // in file as 8 bit
    bool* value;
    qint32 length;

    TArrayBoolean(int len);
    ~TArrayBoolean();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayBoolean* clone() { return new TArrayBoolean(*this); }
    bool getBoolArray(bool** array, int& len);
};

// --------- 2 ------------
class TArrayByte : public TPrimitiveType
{
public:
    char* value;
    qint32 length;

    TArrayByte(int len);
    ~TArrayByte();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayByte* clone() { return new TArrayByte(*this); }
};

// --------- 3 ------------
class TArrayChar : public TPrimitiveType
{
public:
    // unicode char
    // in file as utf-8
    QString value;
    qint32 length;

    TArrayChar(int len);
    ~TArrayChar();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayChar* clone() { return new TArrayChar(*this); }
};

// --------- 5 ------------
class TArrayDecimal : public TPrimitiveType
{
public:
    // in file as string
    double* value;
    qint32 length;

    TArrayDecimal(int len);
    ~TArrayDecimal();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayDecimal* clone() { return new TArrayDecimal(*this); }
    bool getDoubleArray(double** array, int& len);
};

// --------- 6 ------------
class TArrayDouble : public TPrimitiveType
{
public:
    // in file as 64 bit
    double* value;
    qint32 length;

    TArrayDouble(int len);
    ~TArrayDouble();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayDouble* clone() { return new TArrayDouble(*this); }
    bool getDoubleArray(double** array, int& len);
};

// --------- 7 ------------
class TArrayInt16 : public TPrimitiveType
{
public:
    qint16* value;
    qint32 length;

    TArrayInt16(int len);
    ~TArrayInt16();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayInt16* clone() { return new TArrayInt16(*this); }
};

// --------- 8 ------------
class TArrayInt32 : public TPrimitiveType
{
public:
    qint32* value;
    qint32 length;

    TArrayInt32(int len);
    ~TArrayInt32();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayInt32* clone() { return new TArrayInt32(*this); }
    bool getInt32Array(qint32** array, int& len);
};

// --------- 9 ------------
class TArrayInt64 : public TPrimitiveType
{
public:
    qint64* value;
    qint32 length;

    TArrayInt64(int len);
    ~TArrayInt64();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayInt64* clone() { return new TArrayInt64(*this); }
};

// --------- 10 ------------
class TArraySByte : public TPrimitiveType
{
public:
    char* value;
    qint32 length;

    TArraySByte(int len);
    ~TArraySByte();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArraySByte* clone() { return new TArraySByte(*this); }
};

// --------- 11 ------------
class TArraySingle : public TPrimitiveType
{
public:
    // in file as 32 bit float
    float* value;
    qint32 length;

    TArraySingle(int len);
    ~TArraySingle();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArraySingle* clone() { return new TArraySingle(*this); }
};

// --------- 12 ------------
class TArrayTimeSpan : public TPrimitiveType
{
public:
    // in file as 64 bit int
    // specifies duration as int number of 100 nanoseconds
    qint64* value;
    qint32 length;

    TArrayTimeSpan(int len);
    ~TArrayTimeSpan();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayTimeSpan* clone() { return new TArrayTimeSpan(*this); }
};

// --------- 13 ------------
class TArrayDateTime : public TPrimitiveType
{
public:
    // in file as 62 bit int
    // 2 bit time zone: 0 none, 1 utc, 2 local
    // specifies duration as int number of 100 nanoseconds since jan 0001
    qint64* value;
    qint32 length;

    TArrayDateTime(int len);
    ~TArrayDateTime();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayDateTime* clone() { return new TArrayDateTime(*this); }
};

// --------- 14 ------------
class TArrayUInt16 : public TPrimitiveType
{
public:
    quint16* value;
    qint32 length;

    TArrayUInt16(int len);
    ~TArrayUInt16();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayUInt16* clone() { return new TArrayUInt16(*this); }
};

// --------- 15 ------------
class TArrayUInt32 : public TPrimitiveType
{
public:
    quint32* value;
    qint32 length;

    TArrayUInt32(int len);
    ~TArrayUInt32();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayUInt32* clone() { return new TArrayUInt32(*this); }
};

// --------- 16 ------------
class TArrayUInt64 : public TPrimitiveType
{
public:
    quint64* value;
    qint32 length;

    TArrayUInt64(int len);
    ~TArrayUInt64();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayUInt64* clone() { return new TArrayUInt64(*this); }
};

// --------- 17 ------------
class TArrayNull : public TPrimitiveType
{
public:
    qint32 length;

    TArrayNull(int len);
    ~TArrayNull();
    bool read(TBinaryInput&) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayNull* clone() { return new TArrayNull(*this); }
};

// --------- 18 ------------
class TArrayString : public TPrimitiveType
{
public:
    QStringList value;
    qint32 length;

    TArrayString(int len);
    ~TArrayString();
    bool read(TBinaryInput& input) override;
    void write(QTextStream& outstr) override;
    void writeType(QTextStream& outstr) override;
    TArrayString* clone() { return new TArrayString(*this); }
};

// ==========================================
// TBinaryInput
// ==========================================

class TBinaryInput
{
public:
    virtual bool readBytes(char*, int) { return false; }

    int readVariableInt();
    bool readString(QString& str);
    bool readStringWithCode(QString& str);
    bool readArrayOfValueWithCode(QList<TPrimitiveType*>& list);
};

class TFileInput : public TBinaryInput
{
public:
    QFile infile;

    TFileInput(QString& filename);
    ~TFileInput();
    bool open();
    bool readBytes(char* buffer, int count) override;
};

class TFileInputLog : public TBinaryInput
{
public:
    QFile infile;
    QFile logfile;
    int byteCount;

    TFileInputLog(QString& filename, QString& logname);
    ~TFileInputLog();
    bool open();
    bool readBytes(char* buffer, int count) override;
};

// ==========================================
// Stream
// ==========================================

class TStream
{
public:
    TStream();
    ~TStream();

    int read(TBinaryInput& input);
    void write(QTextStream& ts);
    void clearList();
    int recordCount();
    TFileRecord* getRecord(int n);
    qint32 associateReferences();
    TFileRecord* getClass(const QString& name);

private:
    QList<TFileRecord*> recordList;
    TSerializedStreamHeader header;
};


// ==========================================
// Deserializer
// ==========================================

class TDeserializer
{
public:
    TDeserializer();
    ~TDeserializer();

    int read(TBinaryInput& input);
    int streamCount();
    TStream* getStream(int n);
    qint32 associateReferences();
    void getErrorString(int error, QString& str);
    TSearchType* getObject(QStringList& path);
    void getLastPath(QString& path);

private:
    QList<TStream*> streamList;
    QStringList lastPath;

    void clearList();
};

#endif // DESERIALIZER_H
