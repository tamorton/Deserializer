# Deserializer
Reads data from c# binary serialization files

Binary serialization in C# writes a file that can be used to restore the objects and their values. Unfortunately, deserialization in C# needs the class definitions to create instances of the saved classes.

Deserializer is written in C++ and can read the binary file and produce a list of the classes along with the names, types and values of the class members.

The file contains one or more streams. Each call to serialize an object writes a stream. The stream is a series of records, beginning with the header record and ending with the end message record. The format of the records is described in the Microsoft document [MS-NRBF].pdf available online.

You can recognize a file of this type because it usually starts with hexadecimal:
00 01 00 00 00 ff ff ff ff 01 00 00 00 00 00 00 00 0c 02 00 00 00

followed by text, something like: 
Deserializer, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null


For example, the C# code:

	namespace DeserialTest
	{
		[Serializable]
		class TestClass
		{
			public int singleint;
			public int[] intarray;
			public List<SmallClass> classlist;
		}

		[Serializable]
		class SmallClass
		{
			public int smallint;
		}
	}

	TestClass t = new TestClass();
	t.singleint = 123;

	t.intarray = new int[3];
	t.intarray[0] = 10;
	t.intarray[1] = 20;
	t.intarray[2] = 30;

	t.classlist = new List<SmallClass>();
	SmallClass s1 = new SmallClass(); s1.smallint = 100; t.classlist.Add(s1);
	SmallClass s2 = new SmallClass(); s2.smallint = 200; t.classlist.Add(s2);
	SmallClass s3 = new SmallClass(); s3.smallint = 300; t.classlist.Add(s3);

	IFormatter formatter = new BinaryFormatter();
	Stream stream = new FileStream("output.bin", FileMode.Create, FileAccess.Write, FileShare.None);
	formatter.Serialize(stream, t);
	stream.Close();

	
Will write a file output.bin. Opening the file in Deserializer then clicking display will produce the following output:


=== Stream 0 ===
--- Record: 0 ---
Library libraryID:2 libraryName:Deserializer, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null

--- Record: 1 ---
DeserialTest.TestClass (id 1) ClassWithMembersAndTypes
   [0] singleint (int32) 123
   [1] intarray (primitivearray) PrimitiveArray (id 3) length:3
          [0]10 [1]20 [2]30
   [2] classlist (systemclass) System.Collections.Generic.List (id 4) SystemClassWithMembersAndTypes
         [0] _items (class) Array (id 5) rank:1
               [0] length 4
                  [0] DeserialTest.SmallClass (id 6) ClassWithMembersAndTypes
                        [0] smallint (int32) 100
                  [1] DeserialTest.SmallClass (id 7 metadataID 6) ClassWithId
                        [0] smallint (int32) 200
                  [2] DeserialTest.SmallClass (id 8 metadataID 6) ClassWithId
                        [0] smallint (int32) 300
                  [3] null
         [1] _size (int32) 3
         [2] _version (int32) 3

		 
   
An object containing a value can be retrieved by addressing it like a path using a list of strings. The first entry is the stream number. This is followed by the class name then the series of member names to get to the object.

    QStringList path;
    path << "0" << "DeserialTest.TestClass" << "singleint";
    TSearchType* obj = deserial->getObject(path);

Now the value can be read from the object.

	qint64 s;
	obj->getInt(s);
	// s is now 123
	
For arrays of objects, the object can be accessed by an index in the path. (Members can also be accessed by index instead of name.)

    path << "0" << "DeserialTest.TestClass" << "classlist" << "_items" << "1" << "smallint";
    TSearchType* obj = deserial->getObject(path);
	qint64 int1;
	obj->getInt(int1);
	// int1 is now 200

However, elements in primitive arrays can not be accessed by an index in the path. In this case, get the primitive array object then get a pointer to the array from that.	
	
    path << "0" << "DeserialTest.TestClass" << "intarray";
    TSearchType* obj = deserial->getObject(path);
	qint32* a;
	int len;
	obj->getInt32Array(&a, len);
	// a[1] is 20
   
   
The display of lists is truncated based on the value of MAX_ARRAY_PRINT_SIZE.

The input comes through a class called TFileInput. If there are problems reading a file, you can substitute TFileInputLog which writes the bytes to a log file as it reads them.

The code was written using Qt. It has some dependency on Qt types such as QString, QStringList etc but there should be similar things in other libraries.

Reading of the following records has not yet been implemented, so files containing them won't open:

	SystemClassWithMembers
	ClassWithMembers
	MethodCall
	MethodReturn


