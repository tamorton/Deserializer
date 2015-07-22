#include "deserializer.h"

TBinaryType* getBinaryType(char code);
TPrimitiveType* getPrimitiveType(char code);
TPrimitiveType* getArrayType(char code, int len);
TFileRecord* getRecordType(char code);
void indentOutput(QTextStream& outstr, int indent);
TFileRecord* getRecord(qint32 id, QList<TFileRecord*>& recordList);


// ==========================================
// TClassInfo
// ==========================================

bool TClassInfo::read(TBinaryInput& input) {
    if(!input.readBytes((char*)&objectID, 4)) return false;
    if(!input.readString(name)) return false;
    qint32 membercount;
    if(!input.readBytes((char*)&membercount, 4)) return false;
    for(int i=0; i<membercount; i++) {
        QString str;
        if(!input.readString(str)) return false;
        memberNames.append(str);
    }

    // remove [[version info]]
    int pos = name.indexOf("[[");
    if(pos >= 0) {
        displayName = name.left(pos);
    }
    else {
        displayName = name;
    }

    // remove `1
    pos = name.indexOf("`");
    if(pos >= 0) {
        displayName = name.left(pos);
    }

    return true;
}

void TClassInfo::write(QTextStream& outstr, int indent)
{
    outstr << "ClassInfo objectID:" << objectID;
    outstr <<  " displayName:" << displayName;
    outstr <<  "\n";

    for(int i=0; i<memberNames.count(); i++) {
        indentOutput(outstr, indent+1);
        outstr << memberNames.at(i) << "\n";
    }
}

// ==========================================
// PrimitiveTypeEnumeration
// ==========================================

// --------- 1 ------------
bool TPrimitiveBoolean::read(TBinaryInput& input){
    char c;
    if(!input.readBytes(&c, 1)) return false;
    value = (c != 0);
    return true;
}
void TPrimitiveBoolean::write(QTextStream& outstr){

    if(value) {
        outstr << "true";
    }
    else {
        outstr << "false";
    }
}
void TPrimitiveBoolean::writeType(QTextStream& outstr){
    outstr << " (bool) ";
}

// --------- 2 ------------
bool TPrimitiveByte::read(TBinaryInput& input){
    if(!input.readBytes(&value, 1)) return false;
    return true;
}
void TPrimitiveByte::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveByte::writeType(QTextStream& outstr){
    outstr << " (byte) ";
}

// --------- 3 ------------
bool TPrimitiveChar::read(TBinaryInput& input){
    char buffer[6];
    memset(buffer, 0, 6);

    if(!input.readBytes(buffer, 1)) return false;
    int count = 0;
    if((buffer[0] & 0x80) == 0x00) count = 1;
    else if((buffer[0] & 0xE0) == 0xC0) count = 2;
    else if((buffer[0] & 0xF0) == 0xE0) count = 3;
    else if((buffer[0] & 0xF0) == 0xF0) count = 4;
    else return false;

    if(count > 1) {
        if(!input.readBytes(&(buffer[1]), count - 1)) return false;
    }

    value = QString::fromUtf8(buffer, count);
    return true;
}
void TPrimitiveChar::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveChar::writeType(QTextStream& outstr){
    outstr << " (char) ";
}

// --------- 5 ------------
bool TPrimitiveDecimal::read(TBinaryInput& input){
    QString str;
    if(!input.readString(str)) return false;
    value = str.toDouble();
    return true;
}
void TPrimitiveDecimal::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveDecimal::writeType(QTextStream& outstr){
    outstr << " (decimal) ";
}

// --------- 6 ------------
bool TPrimitiveDouble::read(TBinaryInput& input){
    if(!input.readBytes((char*)&value, 8)) return false;
    return true;
}
void TPrimitiveDouble::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveDouble::writeType(QTextStream& outstr){
    outstr << " (double) ";
}

// --------- 7 ------------
bool TPrimitiveInt16::read(TBinaryInput& input){
    if(!input.readBytes((char*)&value, 2)) return false;
    return true;
}
void TPrimitiveInt16::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveInt16::writeType(QTextStream& outstr){
    outstr << " (int16) ";
}

// --------- 8 ------------
bool TPrimitiveInt32::read(TBinaryInput& input){
    if(!input.readBytes((char*)&value, 4)) return false;
    return true;
}
void TPrimitiveInt32::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveInt32::writeType(QTextStream& outstr){
    outstr << " (int32) ";
}

// --------- 9 ------------
bool TPrimitiveInt64::read(TBinaryInput& input){
    if(!input.readBytes((char*)&value, 8)) return false;
    return true;
}
void TPrimitiveInt64::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveInt64::writeType(QTextStream& outstr){
    outstr << " (int64) ";
}

// --------- 10 ------------
bool TPrimitiveSByte::read(TBinaryInput& input){
    if(!input.readBytes(&value, 1)) return false;

    return true;
}
void TPrimitiveSByte::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveSByte::writeType(QTextStream& outstr){
    outstr << " (sbyte) ";
}

// --------- 11 ------------
bool TPrimitiveSingle::read(TBinaryInput& input){
    if(!input.readBytes((char*)&value, 4)) return false;
    return true;
}
void TPrimitiveSingle::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveSingle::writeType(QTextStream& outstr){
    outstr << " (single) ";
}

// --------- 12 ------------
bool TPrimitiveTimeSpan::read(TBinaryInput& input){
    if(!input.readBytes((char*)&value, 8)) return false;
    return true;
}
void TPrimitiveTimeSpan::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveTimeSpan::writeType(QTextStream& outstr){
    outstr << " (time) ";
}

// --------- 13 ------------
bool TPrimitiveDateTime::read(TBinaryInput& input){

    quint64 value;
    if(!input.readBytes((char*)&value, 8)) return false;

    // remove timezone
    value = value & 0x3fffffffffffffff;
    // change from 1/1/0000 to 1/1/1970
    value = value - 0x089f7ff5f7b58000;
    datetime = QDateTime::fromMSecsSinceEpoch(value/10000, Qt::UTC);

    return true;
}
void TPrimitiveDateTime::write(QTextStream& outstr){
    outstr << datetime.toString();
}
void TPrimitiveDateTime::writeType(QTextStream& outstr){
    outstr << " (timedate) ";
}

// --------- 14 ------------
bool TPrimitiveUInt16::read(TBinaryInput& input){
    if(!input.readBytes((char*)&value, 2)) return false;
    return true;
}
void TPrimitiveUInt16::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveUInt16::writeType(QTextStream& outstr){
    outstr << " (uint16) ";
}

// --------- 15 ------------
bool TPrimitiveUInt32::read(TBinaryInput& input){
    if(!input.readBytes((char*)&value, 4)) return false;
    return true;
}
void TPrimitiveUInt32::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveUInt32::writeType(QTextStream& outstr){
    outstr << " (uint32) ";
}

// --------- 16 ------------
bool TPrimitiveUInt64::read(TBinaryInput& input){
    if(!input.readBytes((char*)&value, 8)) return false;
    return true;
}
void TPrimitiveUInt64::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveUInt64::writeType(QTextStream& outstr){
    outstr << " (uint64) ";
}

// --------- 17 ------------
bool TPrimitiveNull::read(TBinaryInput&){
    return true;
}
void TPrimitiveNull::write(QTextStream& outstr){
    outstr << "null";
}
void TPrimitiveNull::writeType(QTextStream& outstr){
    outstr << " (null) ";
}

// --------- 18 ------------
bool TPrimitiveString::read(TBinaryInput& input){
    input.readString(value);
    return true;
}
void TPrimitiveString::write(QTextStream& outstr){
    outstr << value;
}
void TPrimitiveString::writeType(QTextStream& outstr){
    outstr << " (string) ";
}

// ==========================================
// BinaryTypeEnumeration
// ==========================================

TBinaryType::TBinaryType() {
    refRecord = NULL;
    refID = 0;
}

TBinaryType::~TBinaryType() {
}

TBinaryType* TBinaryType::getMember(int &index, QStringList &arglist)
{
    if(refRecord != NULL) return refRecord->getMember(index, arglist);
    return NULL;
}

bool TBinaryType::getString(QString& str)
{
    if(refRecord != NULL) return refRecord->getString(str);
    return false;
}

bool TBinaryType::getBool(bool& result)
{
    if(refRecord != NULL) return refRecord->getBool(result);
    return false;
}

bool TBinaryType::getInt(qint64& result)
{
    if(refRecord != NULL) return refRecord->getInt(result);
    return false;
}

bool TBinaryType::getUnsigned(quint64& result)
{
    if(refRecord != NULL) return refRecord->getUnsigned(result);
    return false;
}

bool TBinaryType::getDouble(double& result)
{
    if(refRecord != NULL) return refRecord->getDouble(result);
    return false;
}

bool TBinaryType::getFloat(float& result)
{
    if(refRecord != NULL) return refRecord->getFloat(result);
    return false;
}

bool TBinaryType::getDateTime(QDateTime& result)
{
    if(refRecord != NULL) return refRecord->getDateTime(result);
    return false;
}

bool TBinaryType::getBoolArray(bool** array, int& len)
{
    if(refRecord != NULL) return refRecord->getBoolArray(array, len);
    return false;
}

bool TBinaryType::getInt32Array(qint32** array, int& len)
{
    if(refRecord != NULL) return refRecord->getInt32Array(array, len);
    return false;
}

bool TBinaryType::getDoubleArray(double** array, int& len)
{
    if(refRecord != NULL) return refRecord->getDoubleArray(array, len);
    return false;
}

bool TBinaryType::getStringArray(QStringList& array)
{
    if(refRecord != NULL) return refRecord->getStringArray(array);
    return false;
}

bool TBinaryType::getObjectArray(TBinaryObject*** array, int& len)
{
    if(refRecord != NULL) return refRecord->getObjectArray(array, len);
    return false;
}

// --------- 0 ------------
TBinaryPrimitive::TBinaryPrimitive() : TBinaryType() {
    value = NULL;
}

TBinaryPrimitive::~TBinaryPrimitive() {
    if(value != NULL) delete value;
}

bool TBinaryPrimitive::read(TBinaryInput& input, QList<TFileRecord*>&){
    if(value != NULL) {
        return(value->read((input)));
    }
    return false;
}

bool TBinaryPrimitive::readAdditional(TBinaryInput& input){
    if(!input.readBytes(&typeEnum, 1)) return false;
    value = getPrimitiveType(typeEnum);
    if(value == NULL) return false;
    return true;
}

void TBinaryPrimitive::write(QTextStream& outstr, int indent){
   if(indent > MAX_INDENT) {outstr << "..."; return;}

   value->write(outstr);
}

void TBinaryPrimitive::writeType(QTextStream& outstr){
   value->writeType(outstr);
}

TBinaryType* TBinaryPrimitive::cloneType() {
    TBinaryPrimitive* b = new TBinaryPrimitive();
    b->value = getPrimitiveType(typeEnum);
    return (TBinaryType*)b;
}

bool TBinaryPrimitive::getBool(bool& result)
{
    if(value == NULL) return false;
    return value->getBool(result);
}

bool TBinaryPrimitive::getInt(qint64& result)
{
    if(value == NULL) return false;
    return value->getInt(result);
}

bool TBinaryPrimitive::getUnsigned(quint64& result)
{
    if(value == NULL) return false;
    return value->getUnsigned(result);
}

bool TBinaryPrimitive::getDouble(double& result)
{
    if(value == NULL) return false;
    return value->getDouble(result);
}

bool TBinaryPrimitive::getFloat(float& result)
{
    if(value == NULL) return false;
    return value->getFloat(result);
}

bool TBinaryPrimitive::getString(QString& result)
{
    if(value == NULL) return false;
    return value->getString(result);
}

bool TBinaryPrimitive::getDateTime(QDateTime& result)
{
    if(value == NULL) return false;
    return value->getDateTime(result);
}

// --------- 1 ------------
TBinaryString::TBinaryString() : TBinaryType() {
}

bool TBinaryString::read(TBinaryInput& input, QList<TFileRecord*>& list){
    char ref;
    if(!input.readBytes(&ref, 1)) return false;
    if(ref == 6) {
        TBinaryObjectString* bos = new TBinaryObjectString();
        bos->type = 6;
        list.append(bos);
        if(!bos->read(input, list)) return false;
        refID = bos->objectID;
        return true;
    }
    if(ref == 9) {
        if(!input.readBytes((char*)&refID, 4)) return false;
        return true;
    }
    if(ref == 0x0a) {
        // null
        return true;
    }
    return false;
}

bool TBinaryString::readNoRef(char ref, TBinaryInput& input, QList<TFileRecord*>& list){
    if(ref == 6) {
        TBinaryObjectString* bos = new TBinaryObjectString();
        bos->type = 6;
        list.append(bos);
        if(!bos->read(input, list)) return false;
        refID = bos->objectID;
        return true;
    }
    if(ref == 9) {
        if(!input.readBytes((char*)&refID, 4)) return false;
        return true;
    }
    if(ref == 0x0a) {
        // null
        return true;
    }
    return false;
}

void TBinaryString::write(QTextStream& outstr, int indent){
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    outstr << value;
    if(refRecord != NULL) {
        refRecord->write(outstr, indent+1);
    }
    else if(refID != 0) {
        outstr << "object " << refID;
    }
}

void TBinaryString::writeType(QTextStream& outstr){
    outstr << " (binary string) ";
}

TBinaryType* TBinaryString::cloneType() {
    TBinaryString* b = new TBinaryString();
    return (TBinaryType*)b;
}

bool TBinaryString::getString(QString& result)
{
    if(refRecord != NULL) {
        // string in ref
        return refRecord->getString(result);
    }
    result = value;
    return true;
}

// --------- 2 ------------
TBinaryObject::TBinaryObject() : TBinaryType() {
}

bool TBinaryObject::read(TBinaryInput& input, QList<TFileRecord*>& recordList){
    char ref;
    if(!input.readBytes(&ref, 1)) return false;

    if(ref == 16) return false;

    if(ref == 9) {
        if(!input.readBytes((char*)&refID, 4)) return false;
        return true;
    }

    refRecord = getRecordType(ref);
    if(refRecord == NULL) return false;
    refRecord->isReferenced = true;
    recordList.append(refRecord);

    if(!refRecord->read(input, recordList)) return false;

    return true;
}

bool TBinaryObject::readNoRef(char ref, TBinaryInput& input, QList<TFileRecord*>& recordList){

    if(ref == 16) return false;

    if(ref == 9) {
        if(!input.readBytes((char*)&refID, 4)) return false;
        return true;
    }

    refRecord = getRecordType(ref);
    if(refRecord == NULL) return false;
    refRecord->isReferenced = true;
    recordList.append(refRecord);

    if(!refRecord->read(input, recordList)) return false;

    return true;
}

void TBinaryObject::write(QTextStream& outstr, int indent){
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    if(refRecord != NULL) {
        refRecord->write(outstr, indent+1);
    }
    else if(refID != 0) {
        outstr << "object " << refID;
    }
    else {
        outstr << "null";
    }
}

void TBinaryObject::writeType(QTextStream& outstr){
    outstr << " (object) ";
}

TBinaryType* TBinaryObject::cloneType() {
    TBinaryObject* b = new TBinaryObject();
    return (TBinaryType*)b;
}

// --------- 3 ------------
TBinarySystemClass::TBinarySystemClass() : TBinaryType() {
}

bool TBinarySystemClass::read(TBinaryInput& input, QList<TFileRecord*>& recordList){
    char ref;
    if(!input.readBytes(&ref, 1)) return false;

    if(ref == 9) {
        if(!input.readBytes((char*)&refID, 4)) return false;
        return true;
    }

    refRecord = getRecordType(ref);
    if(refRecord == NULL) return false;
    refRecord->isReferenced = true;
    recordList.append(refRecord);

    if(!refRecord->read(input, recordList)) return false;

    return true;
}

bool TBinarySystemClass::readAdditional(TBinaryInput& input){
    if(!input.readString(name)) return false;
    return true;
}

void TBinarySystemClass::write(QTextStream& outstr, int indent){
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    if(refRecord != NULL) {
        refRecord->write(outstr, indent+1);
    }
    else if(refID != 0) {
        outstr << "object " << refID;
    }
    else {
        outstr << "null";
    }
}

void TBinarySystemClass::writeType(QTextStream& outstr){
    outstr << " (systemclass) ";
}

TBinaryType* TBinarySystemClass::cloneType() {
    TBinarySystemClass* b = new TBinarySystemClass();
    return (TBinaryType*)b;
}

// --------- 4 ------------
TBinaryClass::TBinaryClass() : TBinaryType() {
}

bool TBinaryClass::read(TBinaryInput& input, QList<TFileRecord*>& recordList){
    char ref;
    if(!input.readBytes(&ref, 1)) return false;

    if(ref == 9) {
        if(!input.readBytes((char*)&refID, 4)) return false;
        return true;
    }

    refRecord = getRecordType(ref);
    if(refRecord == NULL) return false;
    refRecord->isReferenced = true;
    recordList.append(refRecord);

    if(!refRecord->read(input, recordList)) return false;

    return true;
}

bool TBinaryClass::readAdditional(TBinaryInput& input){
    if(!input.readString(name)) return false;
    if(!input.readBytes((char*)&libraryID, 4)) return false;
    return true;
}

void TBinaryClass::write(QTextStream& outstr, int indent){
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    if(refRecord != NULL) {
        refRecord->write(outstr, indent+1);
    }
    else if(refID != 0) {
        outstr << "object " << refID;
    }
    else {
        outstr << "null";
    }
}

void TBinaryClass::writeType(QTextStream& outstr){
    outstr << " (class) ";
}

TBinaryType* TBinaryClass::cloneType() {
    TBinaryClass* b = new TBinaryClass();
    return (TBinaryType*)b;
}

// --------- 5 ------------
TBinaryObjectArray::TBinaryObjectArray() : TBinaryType() {
}

bool TBinaryObjectArray::read(TBinaryInput& input, QList<TFileRecord*>&){
    char ref;
    if(!input.readBytes(&ref, 1)) return false;

    if(ref == 9) {
        if(!input.readBytes((char*)&refID, 4)) return false;
        return true;
    }
    if(ref == 10) {
        // null
        return true;
    }

    // other options not implemented
    return false;
}

void TBinaryObjectArray::write(QTextStream& outstr, int indent){
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    if(refRecord != NULL) {
        refRecord->write(outstr, indent+1);
    }
    else if(refID != 0) {
        outstr << "object " << refID;
    }
    else {
        outstr << "null";
    }
}

void TBinaryObjectArray::writeType(QTextStream& outstr){
    outstr << " (objectarray) ";
}

TBinaryType* TBinaryObjectArray::cloneType() {
    TBinaryObjectArray* b = new TBinaryObjectArray();
    return (TBinaryType*)b;
}

// --------- 6 ------------
TBinaryStringArray::TBinaryStringArray() : TBinaryType() {
}

bool TBinaryStringArray::read(TBinaryInput& input, QList<TFileRecord*>&){
    char ref;
    if(!input.readBytes(&ref, 1)) return false;

    if(ref == 9) {
        if(!input.readBytes((char*)&refID, 4)) return false;
        return true;
    }
    if(ref == 10) {
        // null
        return true;
    }

    // other options not implemented
    return false;
}

void TBinaryStringArray::write(QTextStream& outstr, int indent){
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    if(refRecord != NULL) {
        refRecord->write(outstr, indent+1);
    }
    else if(refID != 0) {
        outstr << "object " << refID;
    }
    else {
        outstr << "null";
    }
}

void TBinaryStringArray::writeType(QTextStream& outstr){
    outstr << " (stringarray) ";
}

TBinaryType* TBinaryStringArray::cloneType() {
    TBinaryStringArray* b = new TBinaryStringArray();
    return (TBinaryType*)b;
}

// --------- 7 ------------
TBinaryPrimitiveArray::TBinaryPrimitiveArray() : TBinaryType() {
}

TBinaryPrimitiveArray::~TBinaryPrimitiveArray() {
}

bool TBinaryPrimitiveArray::read(TBinaryInput& input, QList<TFileRecord*>&){
    char ref;
    if(!input.readBytes(&ref, 1)) return false;

    if(ref == 9) {
        if(!input.readBytes((char*)&refID, 4)) return false;
        return true;
    }
    if(ref == 10) {
        // null
        return true;
    }

    // other options not implemented
    return false;
}

bool TBinaryPrimitiveArray::readAdditional(TBinaryInput& input){
    if(!input.readBytes(&typeEnum, 1)) return false;
    return true;
}

void TBinaryPrimitiveArray::write(QTextStream& outstr, int indent){
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    if(refRecord != NULL) {
        refRecord->write(outstr, indent+1);
    }
    else if(refID != 0) {
        outstr << "object " << refID;
    }
}

void TBinaryPrimitiveArray::writeType(QTextStream& outstr){
    outstr << " (primitivearray) ";
}

TBinaryType* TBinaryPrimitiveArray::cloneType() {
    TBinaryPrimitiveArray* b = new TBinaryPrimitiveArray();
    return (TBinaryType*)b;
}


// ==========================================
// RecordTypeEnumeration
// ==========================================

TFileRecord::TFileRecord() {
    objectID = 0;
    isReferenced = false;
    type = 0;
}

TFileRecord::~TFileRecord() {
    for(int i=0; i<memberList.count(); i++) {
        delete memberList.at(i);
    }
}

void TFileRecord::getReferences(QList<TBinaryType*>& list) {

    for(int i=0; i<memberList.length(); i++) {
        TBinaryType* b = memberList.at(i);
        if(b->refID > 0) {
            list.append(b);
        }
    }
}

TBinaryType* TFileRecord::getMember(int &index, QStringList &arglist)
{
    QString str = arglist.at(index);
    if(str.at(0).isDigit()) {
        // index number
        bool ok;
        int i = str.toInt(&ok, 10);
        if(!ok || i > memberList.length()) return NULL;
        return memberList.at(i);
    }
    else {
        // member name
        for(int i=0; i<classInfo.memberNames.length(); i++) {
            if(classInfo.memberNames.at(i) == str) {
                if(i < memberList.length()) {
                    return memberList.at(i);
                }
            }
        }
    }
    return NULL;
}

// --------- 0 ------------
TSerializedStreamHeader::TSerializedStreamHeader() : TFileRecord() {

}

bool TSerializedStreamHeader::read(TBinaryInput& input, QList<TFileRecord*>&) {
    if(!input.readBytes((char*)&rootID, 4)) return false;
    if(!input.readBytes((char*)&headerID, 4)) return false;
    if(!input.readBytes((char*)&majorVersion, 4)) return false;
    if(!input.readBytes((char*)&minorVersion, 4)) return false;
    classInfo.displayName = "Header";
    classInfo.name = "Header";
    return true;
}

void TSerializedStreamHeader::write(QTextStream& outstr, int)
{
    outstr << "Header rootID:" << rootID << " headerID:" << headerID;
    outstr << " majorVersion:" << majorVersion << " minorVersion:" << minorVersion;
}

// --------- 1 ------------
// class members are defined in object referenced by metadataID

TClassWithId::TClassWithId() : TFileRecord() {
    metadataRecord = NULL;
}

bool TClassWithId::read(TBinaryInput& input, QList<TFileRecord*>& recordList) {
    if(!input.readBytes((char*)&objectID, 4)) return false;
    if(!input.readBytes((char*)&metadataID, 4)) return false;
    classInfo.displayName = "ClassWithId";
    classInfo.name = "ClassWithId";

    // get record containing member info
    metadataRecord = getRecord(metadataID, recordList);
    if(metadataRecord == NULL) return false;

    for(int i=0; i<metadataRecord->memberList.length(); i++) {
        TBinaryType* m = metadataRecord->memberList.at(i)->cloneType();
        memberList.append(m);
        if(!m->read(input, recordList)) return false;

        // copy names to this classinfo
        classInfo.memberNames.append(metadataRecord->classInfo.memberNames.at(i));
    }

    return true;
}

void TClassWithId::write(QTextStream& outstr, int indent)
{
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    if(metadataRecord == NULL) {
        outstr << "ClassWithId (id " << objectID << " metadataID " << metadataID << ") ";
    }
    else {
        outstr << metadataRecord->classInfo.displayName << " (id " << objectID << " metadataID " << metadataID << ") ClassWithId";
    }

    for(int i=0; i<memberList.length(); i++) {
        outstr << "\n";
        indentOutput(outstr, indent + 1);
        outstr << "[" << i << "] ";
        outstr << metadataRecord->classInfo.memberNames.at(i);
        memberList.at(i)->writeType(outstr);
        memberList.at(i)->write(outstr, indent);
    }
}

// --------- 2 ------------
TSystemClassWithMembers::TSystemClassWithMembers() : TFileRecord() {

}

bool TSystemClassWithMembers::read(TBinaryInput& input, QList<TFileRecord*>&) {
    if(!classInfo.read(input)) return false;
    objectID = classInfo.objectID;

    // not implemented -------
    // type of the members is unknown
    // because it should be supplied by the system

    return false;
}

void TSystemClassWithMembers::write(QTextStream& outstr, int indent) {
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    outstr << "SystemClassWithMembers ";
    classInfo.write(outstr, indent + 1);
}

// --------- 3 ------------
TClassWithMembers::TClassWithMembers() : TFileRecord() {

}

bool TClassWithMembers::read(TBinaryInput& input, QList<TFileRecord*>&) {
    if(!classInfo.read(input)) return false;
    objectID = classInfo.objectID;

    if(!input.readBytes((char*)&libraryID, 4)) return false;

    // reading of members
    // not implemented

    return false;
}

void TClassWithMembers::write(QTextStream& outstr, int indent) {
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    outstr << "ClassWithMembers libraryID:" << libraryID << " ";
    classInfo.write(outstr, indent + 1);
}

// --------- 4 ------------
TSystemClassWithMembersAndTypes::TSystemClassWithMembersAndTypes() : TFileRecord() {

}

bool TSystemClassWithMembersAndTypes::read(TBinaryInput& input, QList<TFileRecord*>& recordList) {

    // read class info
    if(!classInfo.read(input)) return false;
    objectID = classInfo.objectID;

    // read member types
    for(int i=0; i<classInfo.memberNames.length(); i++) {
        char type;
        if(!input.readBytes(&type, 1)) return false;

        TBinaryType* binarytype = getBinaryType(type);
        if(binarytype == NULL) return false;

        memberList.append(binarytype);
    }

    // read member additional info
    for(int i=0; i<memberList.count(); i++) {
        if(!memberList.at(i)->readAdditional(input)) return false;
    }

    // read member data
    for(int i=0; i<memberList.count(); i++) {
        if(!memberList.at(i)->read(input, recordList)) return false;
    }

    return true;
}

void TSystemClassWithMembersAndTypes::write(QTextStream& outstr, int indent) {
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    outstr << classInfo.displayName;
    outstr << " (id " << objectID << ") SystemClassWithMembersAndTypes";

    for(int i=0; i<classInfo.memberNames.length(); i++) {
        outstr << "\n";
        indentOutput(outstr, indent + 1);
        outstr << "[" << i << "] ";
        outstr << classInfo.memberNames.at(i);
        memberList.at(i)->writeType(outstr);
        memberList.at(i)->write(outstr, indent+1);
    }
}

// --------- 5 ------------
TClassWithMembersAndTypes::TClassWithMembersAndTypes() : TFileRecord() {
}

TClassWithMembersAndTypes::~TClassWithMembersAndTypes(){
}

bool TClassWithMembersAndTypes::read(TBinaryInput& input, QList<TFileRecord*>& recordList) {

    // read class info
    if(!classInfo.read(input)) return false;
    objectID = classInfo.objectID;

    // read member types
    for(int i=0; i<classInfo.memberNames.length(); i++) {
        char type;
        if(!input.readBytes(&type, 1)) return false;

        TBinaryType* binarytype = getBinaryType(type);
        if(binarytype == NULL) return false;

        memberList.append(binarytype);
    }

    // read member additional info
    for(int i=0; i<memberList.count(); i++) {
        if(!memberList.at(i)->readAdditional(input)) return false;
    }

    // read libraryID
    if(!input.readBytes((char*)&libraryID, 4)) return false;

    // read member data
    for(int i=0; i<memberList.count(); i++) {
        if(!memberList.at(i)->read(input, recordList)) return false;
    }

    return true;
}

void TClassWithMembersAndTypes::write(QTextStream& outstr, int indent) {
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    outstr << classInfo.displayName;
    outstr << " (id " << objectID << ") ClassWithMembersAndTypes";

    for(int i=0; i<classInfo.memberNames.length(); i++) {
        outstr << "\n";
        indentOutput(outstr, indent + 1);
        outstr << "[" << i << "] ";
        outstr << classInfo.memberNames.at(i);
        memberList.at(i)->writeType(outstr);
        memberList.at(i)->write(outstr, indent + 1);
    }
}

// --------- 6 ------------
TBinaryObjectString::TBinaryObjectString() : TFileRecord() {
}

bool TBinaryObjectString::read(TBinaryInput& input, QList<TFileRecord*>&) {
    if(!input.readBytes((char*)&objectID, 4)) return false;
    if(!input.readString(value)) return false;
    classInfo.displayName = "ObjectString";
    classInfo.name = "ObjectString";
    return true;
}

void TBinaryObjectString::write(QTextStream& outstr, int)
{
    outstr << "ObjectString (id" << objectID << ") value:";
    outstr << value;
}

bool TBinaryObjectString::getString(QString& str) {
    str = value;
    return true;
}

// --------- 7 ------------
TBinaryArray::TBinaryArray() : TFileRecord() {
    lengths = NULL;
    lowerBounds = NULL;
    remoteType = NULL;
    array = NULL;
    rank = 0;
    arraySize = 0;
    offset = NULL;
}

TBinaryArray::~TBinaryArray() {

    if(array != NULL) {
        for(int i=0; i<arraySize; i++) {
            if(array[i] != NULL) delete array[i];
        }
        delete[] array;
    }

    if(lengths != NULL) delete[] lengths;
    if(lowerBounds != NULL) delete[] lowerBounds;
    if(remoteType != NULL) delete remoteType;
    if(offset != NULL) delete[] offset;
}


bool TBinaryArray::read(TBinaryInput& input, QList<TFileRecord*>& recordList)
{
    if(!input.readBytes((char*)&objectID, 4)) return false;
    if(!input.readBytes(&arrayType, 1)) return false;
    if(!input.readBytes((char*)&rank, 4)) return false;
    classInfo.displayName = "Array";
    classInfo.name = "Array";

    lengths = new qint32[rank];
    lowerBounds = new qint32[rank];
    offset = new int[rank];

    for(int i=0; i<rank; i++) {
        if(!input.readBytes((char*)&(lengths[i]), 4)) return false;
    }

    if(arrayType == ARRAY_SINGLEOFFSET || arrayType == ARRAY_JAGGEDOFFSET || arrayType == ARRAY_RECTANGULAROFFSET) {
        // only present for SingleOffset, JaggedOffset, or RectangularOffset
        for(int i=0; i<rank; i++) {
            if(!input.readBytes((char*)&(lowerBounds[i]), 4)) return false;
        }
    }

    char type;
    if(!input.readBytes(&type, 1)) return false;

    // single copy to read additional data
    remoteType = getBinaryType(type);
    if(remoteType == NULL) return false;

    if(!remoteType->readAdditional(input)) return false;

    if(arrayType != ARRAY_SINGLE && arrayType != ARRAY_RECTANGULAR) return false;

    if(rank == 0) {
        return true;
    }

    arraySize = 1;
    for(int r=0; r<rank; r++) {
        arraySize *= lengths[r];
        offset[r] = 1;
        for(int s=r+1; s<rank; s++) {
            offset[r] *= lengths[s];
        }
    }
    array = new TBinaryObject*[arraySize];

    int nullcount = 0;

    for(int i=0; i<arraySize; i++) {

        if(nullcount > 0) {
            nullcount--;
            array[i] = NULL;
        }
        else {
            if(!input.readBytes(&type, 1)) return false;

            if(type == 10) {
                // ObjectNull
                array[i] = NULL;
            }
            else if(type == 14) {
                // ObjectNullMultiple
                qint32 n;
                if(!input.readBytes((char*)&n, 4)) return false;
                nullcount = n - 1;
                array[i] = NULL;
            }
            else if(type == 13) {
                // ObjectNullMultiple256
                unsigned char n;
                if(!input.readBytes((char*)&n, 1)) return false;
                nullcount = n - 1;
                array[i] = NULL;
            }
            else {
                array[i] = new TBinaryObject();
                if(!array[i]->readNoRef(type, input, recordList)) return false;
            }
        }
    }

    return true;
}

void TBinaryArray::write(QTextStream& outstr, int indent)
{
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    outstr << "Array (id " << objectID << ") rank:" << rank;

    for(int r=0; r<rank; r++) {
        outstr << "\n";
        indentOutput(outstr, indent+1);
        outstr << "[" << r << "] length " << lengths[r];
    }

    for(int i=0; i<arraySize; i++) {
        outstr << "\n";
        indentOutput(outstr, indent+2);
        outstr << "[" << i << "] ";
        if(array[i] == NULL) {
            outstr << "null";
        }
        else {
            array[i]->write(outstr, indent + 2);
        }

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << "\n";
            indentOutput(outstr, indent+2);
            outstr << "...";
            break;
        }
    }
}

void TBinaryArray::getReferences(QList<TBinaryType*>& list) {

    for(int r=0; r<arraySize; r++) {
        TBinaryType* b = array[r];
        if(b != NULL && b->refID > 0) {
            list.append(b);
        }
    }
}

TBinaryType* TBinaryArray::getMember(int &index, QStringList &arglist)
{
    if(index + rank > arglist.length()) return NULL;

    int i = 0;
    for(int r=0; r<rank; r++) {
        bool ok;
        int d = arglist.at(index+r).toInt(&ok, 10);
        if(!ok) return NULL;

        if(d > lengths[r]) return NULL;

        i += d * offset[r];
    }

    // advance list pointer
    index += rank - 1;

    return array[i];
}

// --------- 8 ------------
TMemberPrimitiveTyped::TMemberPrimitiveTyped() : TFileRecord() {
    value = NULL;
}

TMemberPrimitiveTyped::~TMemberPrimitiveTyped() {
    if(value != NULL) delete value;
}

bool TMemberPrimitiveTyped::read(TBinaryInput& input, QList<TFileRecord*>&) {

    char type;
    if(!input.readBytes(&type, 1)) return false;
    classInfo.displayName = "Primitive";
    classInfo.name = "Primitive";

    value = getPrimitiveType(type);
    if(value == NULL) return false;

    if(!value->read(input)) return false;

    return true;
}

void TMemberPrimitiveTyped::write(QTextStream& outstr, int)
{
    outstr << "Primitive ";
    value->writeType(outstr);
    value->write(outstr);
}

// --------- 9 ------------
TMemberReference::TMemberReference() : TFileRecord() {
}

bool TMemberReference::read(TBinaryInput& input, QList<TFileRecord*>&)
{
    if(!input.readBytes((char*)&(objectID), 4)) return false;
    classInfo.displayName = "Reference";
    classInfo.name = "Reference";

    return true;
}

void TMemberReference::write(QTextStream& outstr, int)
{
    outstr << "Reference refID:" << objectID;
}

void TMemberReference::getReferences(QList<TBinaryType*>&) {
}

// --------- 10 ------------
TObjectNull::TObjectNull() : TFileRecord() {
}

bool TObjectNull::read(TBinaryInput&, QList<TFileRecord*>&) {
    classInfo.displayName = "ObjectNull";
    classInfo.name = "ObjectNull";
    return true;
}

void TObjectNull::write(QTextStream& outstr, int)
{
    outstr << "ObjectNull";
}

// --------- 11 ------------
TMessageEnd::TMessageEnd() : TFileRecord() {
}

bool TMessageEnd::read(TBinaryInput&, QList<TFileRecord*>&) {
    classInfo.displayName = "MessageEnd";
    classInfo.name = "MessageEnd";
    return true;
}

void TMessageEnd::write(QTextStream& outstr, int)
{
    outstr << "MessageEnd";
}

// --------- 12 ------------
TBinaryLibrary::TBinaryLibrary() : TFileRecord() {
}

bool TBinaryLibrary::read(TBinaryInput& input, QList<TFileRecord*>&) {
    if(!input.readBytes((char*)&libraryID, 4)) return false;
    if(!input.readString(libraryName)) return false;
    classInfo.displayName = "Library";
    classInfo.name = "Library";
    return true;
}

void TBinaryLibrary::write(QTextStream& outstr, int)
{
    outstr << "Library libraryID:" << libraryID << " libraryName:";
    outstr << libraryName;
}

// --------- 13 ------------
TObjectNullMultiple256::TObjectNullMultiple256() : TFileRecord() {
}

bool TObjectNullMultiple256::read(TBinaryInput& input, QList<TFileRecord*>&) {
    unsigned char c;
    if(!input.readBytes((char*)&c, 1)) return false;
    classInfo.displayName = "Null";
    classInfo.name = "Null";
    nullCount = c;
    return true;
}

void TObjectNullMultiple256::write(QTextStream& outstr, int)
{
    outstr << "Null count:" << nullCount;
}

// --------- 14 ------------
TObjectNullMultiple::TObjectNullMultiple() : TFileRecord() {
}

bool TObjectNullMultiple::read(TBinaryInput& input, QList<TFileRecord*>&) {
    if(!input.readBytes((char*)&nullCount, 4)) return false;
    classInfo.displayName = "Null";
    classInfo.name = "Null";
    return true;
}

void TObjectNullMultiple::write(QTextStream& outstr, int)
{
    outstr << "Null count:" << nullCount;
}

// --------- 15 ------------
TArraySinglePrimitive::TArraySinglePrimitive() : TFileRecord() {
    array = NULL;
}

TArraySinglePrimitive::~TArraySinglePrimitive() {
    if(array != NULL) delete array;
}

bool TArraySinglePrimitive::read(TBinaryInput& input, QList<TFileRecord*>&) {
    if(!input.readBytes((char*)&objectID, 4)) return false;
    if(!input.readBytes((char*)&length, 4)) return false;
    if(!input.readBytes(&type, 1)) return false;
    array = getArrayType(type, length);
    if(array == NULL) return false;
    if(!array->read(input)) return false;
    classInfo.displayName = "PrimitiveArray";
    classInfo.name = "PrimitiveArray";
    return true;
}

void TArraySinglePrimitive::write(QTextStream& outstr, int indent) {
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    outstr << "PrimitiveArray (id " << objectID << ") length:" << length;

    if(array != NULL) {
        outstr << "\n";
        indentOutput(outstr, indent+1);
        array->write(outstr);
    }
}

bool TArraySinglePrimitive::getBoolArray(bool** a, int& len)
{
    if(array == NULL) return false;
    return (array->getBoolArray(a, len));
}

bool TArraySinglePrimitive::getInt32Array(qint32** a, int& len)
{
    if(array == NULL) return false;
    return (array->getInt32Array(a, len));
}

bool TArraySinglePrimitive::getDoubleArray(double** a, int& len)
{
    if(array == NULL) return false;
    return (array->getDoubleArray(a, len));
}

bool TArraySinglePrimitive::getObjectArray(TBinaryObject*** a, int& len)
{
    if(array == NULL) return false;
    return (array->getObjectArray(a, len));
}

// --------- 16 ------------
TArraySingleObject::TArraySingleObject() : TFileRecord() {
    objectArray = NULL;
    typeArray = NULL;
    doubleArray = NULL;
    int32Array = NULL;
    int64Array = NULL;
    boolArray = NULL;
    length = 0;
}

TArraySingleObject::~TArraySingleObject() {
    if(objectArray != NULL) {
        for(int i=0; i<length; i++) {
            if(objectArray[i] != NULL) delete objectArray[i];
        }
        delete[] objectArray;
    }
    if(typeArray != NULL) delete[] typeArray;
    if(doubleArray != NULL) delete[] doubleArray;
    if(int32Array != NULL) delete[] int32Array;
    if(int64Array != NULL) delete[] int64Array;
    if(boolArray != NULL) delete[] boolArray;
}

bool TArraySingleObject::read(TBinaryInput& input, QList<TFileRecord*>& recordList) {
    if(!input.readBytes((char*)&objectID, 4)) return false;
    if(!input.readBytes((char*)&length, 4)) return false;
    classInfo.displayName = "ArraySingleObject";
    classInfo.name = "ArraySingleObject";

    if(length == 0) return true;

    // 0 for null
    // primitive type 6 - double, 8 - int32
    // -1 for object

    typeArray = new char[length];
    memset(typeArray, 0, length*sizeof(char));

    // objects loaded here to add multiple nulls to count
    for(int m=0; m<length; m++) {
        char ref;
        if(!input.readBytes(&ref, 1)) return false;

        // no arrays within arrays
        if(ref == 16) return false;

        if(ref == 13) {
            // ObjectNullMultiple256
            unsigned char n;
            if(!input.readBytes((char*)&n, 1)) return false;

            if(objectArray != NULL) {
                for(int i=0; i<n; i++) {
                    objectArray[m+i] = NULL;
                }
            }
            m += n;
        }
        else if(ref == 14) {
            // ObjectNullMultiple
            qint32 n;
            if(!input.readBytes((char*)&n, 4)) return false;

            if(objectArray != NULL) {
                for(int i=0; i<n; i++) {
                    objectArray[m+i] = NULL;
                }
            }
            m += n;
        }
        else if(ref == 8) {
            // primitive
            char type;
            if(!input.readBytes((char*)&type, 1)) return false;

            typeArray[m] = type;

            if(type == 1) {
                // bool
                if(boolArray == NULL) {
                    boolArray = new bool[length];
                    memset(boolArray, 0, length*sizeof(bool));
                }
                char inp;
                if(!input.readBytes((char*)&inp, 1)) return false;
                boolArray[m] = (inp != 0);
            }
            else if(type == 6) {
                // double
                if(doubleArray == NULL) {
                    doubleArray = new double[length];
                    memset(doubleArray, 0, length*sizeof(double));
                }
                if(!input.readBytes((char*)&(doubleArray[m]), 8)) return false;
            }
            else if(type == 8) {
                // int32
                if(int32Array == NULL) {
                    int32Array = new qint32[length];
                    memset(int32Array, 0, length*sizeof(qint32));
                }
                if(!input.readBytes((char*)&(int32Array[m]), 4)) return false;
            }
            else if(type == 9) {
                // int64
                if(int64Array == NULL) {
                    int64Array = new qint64[length];
                    memset(int64Array, 0, length*sizeof(qint64));
                }
                if(!input.readBytes((char*)&(int64Array[m]), 8)) return false;
            }

            // type not implemented
            else return false;

        }
        else {
            // reference to object
            if(objectArray == NULL) {
                objectArray = new TBinaryObject*[length];
                for(int i=0; i<length; i++) {
                    objectArray[i] = NULL;
                }
            }

            objectArray[m] = new TBinaryObject();
            if(!objectArray[m]->readNoRef(ref, input, recordList)) return false;

            typeArray[m] = -1;
        }
    }

    return true;
}

void TArraySingleObject::write(QTextStream& outstr, int indent)
{
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    outstr << "ArraySingleObject (id " << objectID << ") length:" << length;

    for(int i=0; i<length; i++) {
        outstr << "\n";

        indentOutput(outstr, indent + 1);
        outstr << "[" << i << "] ";

        if(typeArray[i] == 0) {
            outstr << "null";
        }
        else if(typeArray[i] == 1 && boolArray != NULL){
            if(boolArray[i]) outstr << "true";
            else outstr << "false";
        }
        else if(typeArray[i] == 6 && doubleArray != NULL){
            outstr << doubleArray[i];
        }
        else if(typeArray[i] == 8 && int32Array != NULL){
            outstr << int32Array[i];
        }
        else if(typeArray[i] == 9 && int64Array != NULL){
            outstr << int64Array[i];
        }
        else if(typeArray[i] == -1 && objectArray != NULL){
            if(objectArray[i] == NULL) outstr << "null";
            else objectArray[i]->write(outstr, indent+1);
        }

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << "\n";
            indentOutput(outstr, indent+1);
            outstr << "...";
            break;
        }
    }
}

void TArraySingleObject::getReferences(QList<TBinaryType*>& btlist)
{
    if(typeArray != NULL && objectArray != NULL) {
        for(int i=0; i<length; i++) {
            if(typeArray[i] == -1 && objectArray[i] != NULL) {
                if(objectArray[i]->refID > 0) {
                    btlist.append(objectArray[i]);
                }
            }
        }
    }
}

TBinaryType* TArraySingleObject::getMember(int &index, QStringList &arglist)
{
    // to avoid having to create binarytype from primitives
    // this only works if it is an array of objects
    // for primitives use getArray to get the whole array

    QString str = arglist.at(index);
    if(str.at(0).isDigit()) {
        // index number
        bool ok;
        int i = str.toInt(&ok, 10);
        if(!ok || i > length || objectArray == NULL) return NULL;
        return objectArray[i];
    }
    return NULL;
}

bool TArraySingleObject::getBoolArray(bool** array, int& len)
{
    len = length;
    *array = boolArray;
    return (boolArray != NULL);
}

bool TArraySingleObject::getInt32Array(qint32** array, int& len)
{
    len = length;
    *array = int32Array;
    return (int32Array != NULL);
}

bool TArraySingleObject::getDoubleArray(double** array, int& len)
{
    len = length;
    *array = doubleArray;
    return (doubleArray != NULL);
}

bool TArraySingleObject::getObjectArray(TBinaryObject*** array, int& len)
{
    len = length;
    *array = objectArray;
    return (objectArray != NULL);
}

// --------- 17 ------------
TArraySingleString::TArraySingleString() : TFileRecord() {
}

TArraySingleString::~TArraySingleString() {
}

bool TArraySingleString::read(TBinaryInput& input, QList<TFileRecord*>& recordList) {
    if(!input.readBytes((char*)&objectID, 4)) return false;
    qint32 length;
    if(!input.readBytes((char*)&length, 4)) return false;
    classInfo.displayName = "ArraySingleString";
    classInfo.name = "ArraySingleString";

    // objects loaded here to add multiple nulls to count
    for(int m=0; m<length; m++) {
        char ref;
        if(!input.readBytes(&ref, 1)) return false;

        // no arrays within arrays
        if(ref == 16) return false;

        if(ref == 13) {
            // ObjectNullMultiple256
            unsigned char n;
            if(!input.readBytes((char*)&n, 1)) return false;
            m += n;
        }
        else if(ref == 14) {
            // ObjectNullMultiple
            qint32 n;
            if(!input.readBytes((char*)&n, 4)) return false;
            m += n;
        }
        else {
            TBinaryString* str = new TBinaryString();
            str->readNoRef(ref, input, recordList);
            memberList.append(str);
        }
    }

    return true;
}

void TArraySingleString::write(QTextStream& outstr, int indent)
{
    if(indent > MAX_INDENT) {outstr << "..."; return;}

    outstr << "ArraySingleString (id " << objectID << ") length:" << memberList.length() << "\n";

    for(int i=0; i<memberList.length(); i++) {
        indentOutput(outstr, indent+1);
        outstr << "[" << i << "] ";
        memberList.at(i)->write(outstr, indent+1);
    }
}

bool TArraySingleString::getStringArray(QStringList& list)
{
    for(int i=0; i<memberList.length(); i++) {
        QString str;
        memberList.at(i)->getString(str);
        list.append(str);
    }
    return true;
}

// --------- 21 ------------
TMethodCall::TMethodCall() : TFileRecord() {
}

bool TMethodCall::read(TBinaryInput& input, QList<TFileRecord*>&) {
    if(!input.readBytes((char*)&messageFlags, 4)) return false;
    if(!input.readStringWithCode(methodName)) return false;
    if(!input.readStringWithCode(typeName)) return false;
    classInfo.displayName = "MethodCall";
    classInfo.name = "MethodCall";

    if((messageFlags & MESSAGE_CONTEXTINLINE) != 0) {
        if(!input.readStringWithCode(callContext)) return false;
    }
    if((messageFlags & MESSAGE_ARGSINLINE) != 0) {
        if(!input.readArrayOfValueWithCode(args)) return false;
    }
    return false;
}

void TMethodCall::write(QTextStream& outstr, int)
{
    outstr << "MethodCall " << methodName;
}

// --------- 22 ------------
TMethodReturn::TMethodReturn() : TFileRecord() {
}

bool TMethodReturn::read(TBinaryInput& input, QList<TFileRecord*>&) {
    if(!input.readBytes((char*)&messageFlags, 4)) return false;
    if(!input.readStringWithCode(methodName)) return false;
    if(!input.readStringWithCode(typeName)) return false;
    classInfo.displayName = "MethodReturn";
    classInfo.name = "MethodReturn";

    if((messageFlags & MESSAGE_CONTEXTINLINE) != 0) {
        if(!input.readStringWithCode(callContext)) return false;
    }
    if((messageFlags & MESSAGE_ARGSINLINE) != 0) {
        if(!input.readArrayOfValueWithCode(args)) return false;
    }
    return false;
}

void TMethodReturn::write(QTextStream& outstr, int)
{
    outstr << "MethodReturn " << methodName;
}

// ==========================================
// ArrayTypeEnumeration
// ==========================================

// --------- 1 ------------
TArrayBoolean::TArrayBoolean(int len) {
    length = len;
    value = new bool[len];
}

TArrayBoolean::~TArrayBoolean() {
    delete[] value;
}

bool TArrayBoolean::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        char b;
        if(!input.readBytes(&b, 1)) return false;
        value[i] = (b != 0);
    }
    return true;
}

void TArrayBoolean::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        if(value[i]) outstr << " [" << i << "]:true";
        else outstr << " [" << i << "]:false";

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArrayBoolean::writeType(QTextStream& outstr){
    outstr << " (bool array) ";
}

bool TArrayBoolean::getBoolArray(bool** array, int& len)
{
    len = length;
    *array = value;
    return true;
}

// --------- 2 ------------
TArrayByte::TArrayByte(int len) {
    length = len;
    value = new char[len];
}

TArrayByte::~TArrayByte() {
    delete[] value;
}

bool TArrayByte::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        if(!input.readBytes(&(value[i]), 1)) return false;
    }
    return true;
}
void TArrayByte::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}
void TArrayByte::writeType(QTextStream& outstr){
    outstr << " (byte array) ";
}

// --------- 3 ------------
TArrayChar::TArrayChar(int len) {
    length = len;
    value.reserve(len);
}

TArrayChar::~TArrayChar() {

}

bool TArrayChar::read(TBinaryInput&){
    return false;
}
void TArrayChar::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}
void TArrayChar::writeType(QTextStream& outstr){
    outstr << " (char array) ";
}

// --------- 5 ------------
TArrayDecimal::TArrayDecimal(int len) {
    length = len;
    value = new double[len];
}

TArrayDecimal::~TArrayDecimal() {
    delete[] value;
}

bool TArrayDecimal::read(TBinaryInput&){
    return false;
}

void TArrayDecimal::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArrayDecimal::writeType(QTextStream& outstr){
    outstr << " (decimal array) ";
}

bool TArrayDecimal::getDoubleArray(double** array, int& len)
{
    len = length;
    *array = value;
    return true;
}

// --------- 6 ------------
TArrayDouble::TArrayDouble(int len) {
    length = len;
    value = new double[len];
}

TArrayDouble::~TArrayDouble() {
    delete[] value;
}

bool TArrayDouble::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        if(!input.readBytes((char*)&(value[i]), 8)) return false;
    }
    return true;
}

void TArrayDouble::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArrayDouble::writeType(QTextStream& outstr){
    outstr << " (double array) ";
}

bool TArrayDouble::getDoubleArray(double** array, int& len)
{
    len = length;
    *array = value;
    return true;
}

// --------- 7 ------------
TArrayInt16::TArrayInt16(int len) {
    length = len;
    value = new qint16[len];
}

TArrayInt16::~TArrayInt16() {
    delete[] value;
}

bool TArrayInt16::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        if(!input.readBytes((char*)&(value[i]), 2)) return false;
    }
    return true;
}

void TArrayInt16::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArrayInt16::writeType(QTextStream& outstr){
    outstr << " (int16 array) ";
}

// --------- 8 ------------
TArrayInt32::TArrayInt32(int len) {
    length = len;
    value = new qint32[len];
}

TArrayInt32::~TArrayInt32() {
    delete[] value;
}

bool TArrayInt32::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        if(!input.readBytes((char*)&(value[i]), 4)) return false;
    }
    return true;
}

void TArrayInt32::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArrayInt32::writeType(QTextStream& outstr){
    outstr << " (int32 array) ";
}

bool TArrayInt32::getInt32Array(qint32** array, int& len)
{
    len = length;
    *array = value;
    return true;
}

// --------- 9 ------------
TArrayInt64::TArrayInt64(int len) {
    length = len;
    value = new qint64[len];
}

TArrayInt64::~TArrayInt64() {
    delete[] value;
}

bool TArrayInt64::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        if(!input.readBytes((char*)&(value[i]), 8)) return false;
    }
    return true;
}

void TArrayInt64::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArrayInt64::writeType(QTextStream& outstr){
    outstr << " (int64 array) ";
}

// --------- 10 ------------
TArraySByte::TArraySByte(int len) {
    length = len;
    value = new char[len];
}

TArraySByte::~TArraySByte() {
    delete[] value;
}

bool TArraySByte::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        if(!input.readBytes(&(value[i]), 1)) return false;
    }
    return true;
}

void TArraySByte::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArraySByte::writeType(QTextStream& outstr){
    outstr << " (sbyte array) ";
}

// --------- 11 ------------
TArraySingle::TArraySingle(int len) {
    length = len;
    value = new float[len];
}

TArraySingle::~TArraySingle() {
    delete[] value;
}

bool TArraySingle::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        if(!input.readBytes((char*)&(value[i]), 4)) return false;
    }
    return true;
}

void TArraySingle::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArraySingle::writeType(QTextStream& outstr){
    outstr << " (single array) ";
}

// --------- 12 ------------
TArrayTimeSpan::TArrayTimeSpan(int len) {
    length = len;
    value = new qint64[len];
}

TArrayTimeSpan::~TArrayTimeSpan() {
    delete[] value;
}

bool TArrayTimeSpan::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        if(!input.readBytes((char*)&(value[i]), 8)) return false;
    }
    return true;
}

void TArrayTimeSpan::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArrayTimeSpan::writeType(QTextStream& outstr){
    outstr << " (time array) ";
}

// --------- 13 ------------
TArrayDateTime::TArrayDateTime(int len) {
    length = len;
    value = new qint64[len];
}

TArrayDateTime::~TArrayDateTime() {
    delete[] value;
}

bool TArrayDateTime::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        if(!input.readBytes((char*)&(value[i]), 8)) return false;
    }
    return true;
}

void TArrayDateTime::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArrayDateTime::writeType(QTextStream& outstr){
    outstr << " (timedate array) ";
}

// --------- 14 ------------
TArrayUInt16::TArrayUInt16(int len) {
    length = len;
    value = new quint16[len];
}

TArrayUInt16::~TArrayUInt16() {
    delete[] value;
}

bool TArrayUInt16::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        if(!input.readBytes((char*)&(value[i]), 2)) return false;
    }
    return true;
}

void TArrayUInt16::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArrayUInt16::writeType(QTextStream& outstr){
    outstr << " (uint16 array) ";
}

// --------- 15 ------------
TArrayUInt32::TArrayUInt32(int len) {
    length = len;
    value = new quint32[len];
}

TArrayUInt32::~TArrayUInt32() {
    delete[] value;
}

bool TArrayUInt32::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        if(!input.readBytes((char*)&(value[i]), 4)) return false;
    }
    return true;
}

void TArrayUInt32::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArrayUInt32::writeType(QTextStream& outstr){
    outstr << " (uint32 array) ";
}

// --------- 16 ------------
TArrayUInt64::TArrayUInt64(int len) {
    length = len;
    value = new quint64[len];
}

TArrayUInt64::~TArrayUInt64() {
    delete[] value;
}

bool TArrayUInt64::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        if(!input.readBytes((char*)&(value[i]), 8)) return false;
    }
    return true;
}

void TArrayUInt64::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}

void TArrayUInt64::writeType(QTextStream& outstr){
    outstr << " (uint64 array) ";
}

// --------- 17 ------------
TArrayNull::TArrayNull(int len) {
    length = len;
}

TArrayNull::~TArrayNull() {

}

bool TArrayNull::read(TBinaryInput&){
    return false;
}
void TArrayNull::write(QTextStream& outstr){
    outstr << length << " x null";
}
void TArrayNull::writeType(QTextStream& outstr){
    outstr << " (null array) ";
}

// --------- 18 ------------
TArrayString::TArrayString(int len) {
    length = len;
    value.reserve(len);
}

TArrayString::~TArrayString() {

}

bool TArrayString::read(TBinaryInput& input){
    for(int i=0; i<length; i++) {
        QString str;
        if(!input.readString(str)) return false;
        value.append(str);
    }
    return true;
}
void TArrayString::write(QTextStream& outstr)
{
    for(int i=0; i<length; i++) {
        outstr << " [" << i << "]" << value[i];

        if(i >= MAX_ARRAY_PRINT_SIZE) {
            outstr << " ...";
            break;
        }
    }
}
void TArrayString::writeType(QTextStream& outstr){
    outstr << " (string array) ";
}

// ==========================================
// TStream
// ==========================================

TStream::TStream()
{

}

TStream::~TStream()
{
    clearList();
}

int TStream::read(TBinaryInput& input)
{
    clearList();

    // read remainder of header
    if(!header.read(input, recordList)) return DESERIAL_RECORD_READ_FAILED;

    char c = 0;
    while(c != 0x0b) {

        if(!input.readBytes(&c, 1)) return DESERIAL_RECORD_READ_FAILED;
        if(c != 0x0b) {
            // don't include end message record
            TFileRecord* record = getRecordType(c);
            if(record == NULL) {
                return DESERIAL_UNKNOWN_RECORD;
            }

            if(!record->read(input, recordList)) {
                return DESERIAL_RECORD_READ_FAILED;
            }

            recordList.append(record);
        }
    }

    return DESERIAL_OK;
}

void TStream::write(QTextStream& ts)
{
    for(int i=0; i<recordList.size(); i++) {
        if(i == 66) {
            i = 66;
        }
        TFileRecord* r = recordList.at(i);
        if(r != NULL && !r->isReferenced) {
            ts << "--- Record: " << i << " ---\n";
            r->write(ts, 0);
            ts << "\n\n";
        }
    }
}

void TStream::clearList()
{
    for (int i = 0; i < recordList.size(); ++i) {
        TFileRecord* record = recordList.at(i);
        delete record;
    }
    recordList.clear();
}

int TStream::recordCount()
{
    return recordList.count();
}

TFileRecord* TStream::getRecord(int n)
{
    if(n >= recordList.count()) return NULL;
    return recordList.at(n);
}

qint32 TStream::associateReferences()
{
    // find record with object id referenced by member
    // within same stream
    // returns record not found or 0 on success

    QList<TBinaryType*> btlist;
    for (int i = 0; i < recordList.size(); ++i) {
        TFileRecord* record = recordList.at(i);
        record->getReferences(btlist);
    }

    for(int i=0; i<btlist.size(); i++) {
        TBinaryType* b = btlist.at(i);
        bool found = false;
        for (int j = 0; j < recordList.size(); ++j) {
            TFileRecord* testrec = recordList.at(j);
            if(testrec->objectID == b->refID) {
                b->refRecord = testrec;
                testrec->isReferenced = true;
                found = true;
                break;
            }
        }
        if(!found) return b->refID;
    }

    return 0;
}

TFileRecord* TStream::getClass(const QString& name)
{
    for (int i = 0; i < recordList.size(); ++i) {
        TFileRecord* r = recordList.at(i);
        if(r != NULL && !r->isReferenced) {
            if(r->classInfo.displayName == name) {
                return r;
            }
        }
    }
    return NULL;
}


// ==========================================
// TDeserializer
// ==========================================

TDeserializer::TDeserializer()
{

}

TDeserializer::~TDeserializer()
{
    clearList();
}

int TDeserializer::read(TBinaryInput& input)
{
    clearList();

    char c;
    if(!input.readBytes(&c, 1)) {
        return DESERIAL_FILE_READ_FAILED;
    }

    bool finished = false;
    while(!finished) {

        if(c != 0) {
            return DESERIAL_NO_HEADER;
        }

        TStream* stream = new TStream();
        streamList.append(stream);

        int result = stream->read(input);
        if(result != DESERIAL_OK) {
            return result;
        }

        finished = (!input.readBytes(&c, 1));
    }

    return DESERIAL_OK;
}

void TDeserializer::clearList()
{
    for (int i = 0; i < streamList.size(); ++i) {
        TStream* stream = streamList.at(i);
        delete stream;
    }
    streamList.clear();
}

qint32 TDeserializer::associateReferences()
{
    // find record with object id referenced by member
    for (int i = 0; i < streamList.size(); ++i) {
        qint32 result = streamList.at(i)->associateReferences();
        if(result != 0) return result;
    }
    return 0;
}

int TDeserializer::streamCount()
{
    return streamList.count();
}

TStream* TDeserializer::getStream(int n)
{
    if(n >= streamList.count()) return NULL;
    return streamList.at(n);
}

TSearchType* TDeserializer::getObject(QStringList& path)
{
    // 0:stream number
    // 1:record class displayname
    // 2:member name or index(number)
    // ...:member name or index(number)
    lastPath = path;

    if(path.size() < 3) return NULL;

    // stream
    bool ok;
    int s = path.at(0).toInt(&ok, 10);
    if(!ok) return NULL;

    TStream* stream = getStream(s);
    if(stream == NULL) return NULL;

    // class
    TSearchType* result = stream->getClass(path.at(1));
    if(result == NULL) return NULL;

    for(int p=2; p<path.size(); p++) {
        result = result->getMember(p, path);
        if(result == NULL) return NULL;
    }
    return result;
}

void TDeserializer::getLastPath(QString& path)
{
    path = "";
    for(int s=0; s<lastPath.length(); s++) {
        if(s > 0) path += "/";
        path += lastPath.at(s);
    }
}

void TDeserializer::getErrorString(int error, QString& str)
{
    switch(error) {
        case DESERIAL_OK: str = "Ok"; break;
        case DESERIAL_FILE_OPEN_FAILED: str = "File open failed"; break;
        case DESERIAL_FILE_READ_FAILED: str = "File read failed"; break;
        case DESERIAL_NO_HEADER: str = "Header not found"; break;
        case DESERIAL_UNKNOWN_RECORD: str = "Unknown record"; break;
        case DESERIAL_RECORD_READ_FAILED: str = "Failed to read record"; break;
        default: str = "Unknown error";
    }
}


// ==========================================
// TBinaryInput
// ==========================================

int TBinaryInput::readVariableInt() {
    int result = 0;
    for(int i=0; i<5; i++) {
        char c;
        if(!readBytes(&c, 1)) {
            return result;
        }
        result += ((c & 0x7F) << (i * 7));
        if((c & 0x80) == 0) {
            // last byte
            break;
        }
    }
    return result;
}

bool TBinaryInput::readString(QString& str) {
    int strsize = readVariableInt();
    if(strsize == 0) {
        str = "";
        return true;
    }

    char* buffer = new char[strsize];
    if(!readBytes(buffer, strsize)) return false;
    str = QString::fromUtf8(buffer, strsize);
    delete[] buffer;

    return true;
}

bool TBinaryInput::readStringWithCode(QString& str) {
    char c; // 18 for string
    if(!readBytes(&c, 1) || c != 18) {
        return false;
    }

    int strsize = readVariableInt();

    char* buffer = new char[strsize];
    if(!readBytes(buffer, strsize)) return false;
    str = QString::fromUtf8(buffer, strsize);
    delete[] buffer;

    return true;
}

bool TBinaryInput::readArrayOfValueWithCode(QList<TPrimitiveType*>& list)
{
    qint32 length;
    if(!readBytes((char*)&length, 4)) return false;

    for(int i=0; i<length; i++) {
        char c;
        if(!readBytes(&c, 1)) return false;

        TPrimitiveType* p = getPrimitiveType(c);
        if(p == NULL) return false;

        p->read(*this);
        list.append(p);
    }
    return true;
}

// ==========================================
// TFileInput
// ==========================================

TFileInput::TFileInput(QString& filename) : infile(filename)
{

}

TFileInput::~TFileInput()
{

}

bool TFileInput::open()
{
    return infile.open(QFile::ReadOnly);
}

bool TFileInput::readBytes(char* buffer, int count)
{
    return(infile.read(buffer, count) == count);
}

// ==========================================
// TFileInputLog
// ==========================================

TFileInputLog::TFileInputLog(QString& filename, QString &logname)
    : infile(filename), logfile(logname)
{
    byteCount = 0;
}

TFileInputLog::~TFileInputLog()
{
    logfile.close();
}

bool TFileInputLog::open()
{
    return (infile.open(QFile::ReadOnly) && logfile.open(QIODevice::WriteOnly));
}

bool TFileInputLog::readBytes(char* buffer, int count)
{
    if(!(infile.read(buffer, count) == count)) return false;

    QString line;
    line.sprintf("%.6d  ", byteCount);
    byteCount += count;

    QString hex;
    for(int i=0; i<count; i++) {
        hex.sprintf("%.2x ", (unsigned char)buffer[i]);
        line += hex;
    }
    line += "\n";
    logfile.write(line.toUtf8());
    return true;
}

// ==========================================
// ==========================================

TFileRecord* getRecordType(char code) {

    TFileRecord* record = NULL;

    switch(code) {
        case 0:
            record = (TFileRecord*)new TSerializedStreamHeader();
            break;
        case 1:
            record = (TFileRecord*)new TClassWithId();
            break;
        case 2:
            record = (TFileRecord*)new TSystemClassWithMembers();
            break;
        case 3:
            record = (TFileRecord*)new TClassWithMembers();
            break;
        case 4:
            record = (TFileRecord*)new TSystemClassWithMembersAndTypes();
            break;
        case 5:
            record = (TFileRecord*)new TClassWithMembersAndTypes();
            break;
        case 6:
            record = (TFileRecord*)new TBinaryObjectString();
            break;
        case 7:
            record = (TFileRecord*)new TBinaryArray();
            break;
        case 8:
            record = (TFileRecord*)new TMemberPrimitiveTyped();
            break;
        case 9:
            record = (TFileRecord*)new TMemberReference();
            break;
        case 10:
            record = (TFileRecord*)new TObjectNull();
            break;
        case 11:
            record = (TFileRecord*)new TMessageEnd();
            break;
        case 12:
            record = (TFileRecord*)new TBinaryLibrary();
            break;
        case 13:
            record = (TFileRecord*)new TObjectNullMultiple256();
            break;
        case 14:
            record = (TFileRecord*)new TObjectNullMultiple();
            break;
        case 15:
            record = (TFileRecord*)new TArraySinglePrimitive();
            break;
        case 16:
            record = (TFileRecord*)new TArraySingleObject();
            break;
        case 17:
            record = (TFileRecord*)new TArraySingleString();
            break;
        case 21:
            record = (TFileRecord*)new TMethodCall();
            break;
        case 22:
            record = (TFileRecord*)new TMethodReturn();
            break;
    }
    if(record != NULL) record->type = code;
    return record;
}

TBinaryType* getBinaryType(char code) {

    TBinaryType* result = NULL;
    switch(code) {
        case 0:
            result = new TBinaryPrimitive();
            break;
        case 1:
            result = new TBinaryString();
            break;
        case 2:
            result = new TBinaryObject();
            break;
        case 3:
            result = new TBinarySystemClass();
            break;
        case 4:
            result = new TBinaryClass();
            break;
        case 5:
            result = new TBinaryObjectArray();
            break;
        case 6:
            result = new TBinaryStringArray();
            break;
        case 7:
            result = new TBinaryPrimitiveArray();
            break;
    }
    return result;
}

TPrimitiveType* getPrimitiveType(char code) {

    TPrimitiveType* result = NULL;
    switch(code) {
        case 1:
            result = new TPrimitiveBoolean();
            break;
        case 2:
            result = new TPrimitiveByte();
            break;
        case 3:
            result = new TPrimitiveChar();
            break;
        case 5:
            result = new TPrimitiveDecimal();
            break;
        case 6:
            result = new TPrimitiveDouble();
            break;
        case 7:
            result = new TPrimitiveInt16();
            break;
        case 8:
            result = new TPrimitiveInt32();
            break;
        case 9:
            result = new TPrimitiveInt64();
            break;
        case 10:
            result = new TPrimitiveSByte();
            break;
        case 11:
            result = new TPrimitiveSingle();
            break;
        case 12:
            result = new TPrimitiveTimeSpan();
            break;
        case 13:
            result = new TPrimitiveDateTime();
            break;
        case 14:
            result = new TPrimitiveUInt16();
            break;
        case 15:
            result = new TPrimitiveUInt32();
            break;
        case 16:
            result = new TPrimitiveUInt64();
            break;
        case 17:
            result = new TPrimitiveNull();
            break;
        case 18:
            result = new TPrimitiveString();
            break;
    }
    return result;
}

TPrimitiveType *getArrayType(char code, int len) {

    TPrimitiveType* result = NULL;
    switch(code) {
        case 1:
            result = new TArrayBoolean(len);
            break;
        case 2:
            result = new TArrayByte(len);
            break;
        case 3:
            result = new TArrayChar(len);
            break;
        case 5:
            result = new TArrayDecimal(len);
            break;
        case 6:
            result = new TArrayDouble(len);
            break;
        case 7:
            result = new TArrayInt16(len);
            break;
        case 8:
            result = new TArrayInt32(len);
            break;
        case 9:
            result = new TArrayInt64(len);
            break;
        case 10:
            result = new TArraySByte(len);
            break;
        case 11:
            result = new TArraySingle(len);
            break;
        case 12:
            result = new TArrayTimeSpan(len);
            break;
        case 13:
            result = new TArrayDateTime(len);
            break;
        case 14:
            result = new TArrayUInt16(len);
            break;
        case 15:
            result = new TArrayUInt32(len);
            break;
        case 16:
            result = new TArrayUInt64(len);
            break;
        case 17:
            result = new TArrayNull(len);
            break;
        case 18:
            result = new TArrayString(len);
            break;
    }
    return result;
}

void indentOutput(QTextStream& outstr, int indent)
{
    for(int i=0; i<indent; i++) {
        outstr << "   ";
    }
}

TFileRecord* getRecord(qint32 id, QList<TFileRecord*>& recordList)
{
    for(int i=0; i<recordList.length(); i++) {
        TFileRecord* record = recordList.at(i);
        if(record->objectID == id) return record;
    }
    return NULL;
}

