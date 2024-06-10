#include <iostream>
#include <cmath>
#include "ghjson.hpp"

using namespace std;
int succ = 0;
int count  = 0;
double epsilon = 1e-5;

void TestparseObject(ghjson::object expect, const string jsonStr)
{
    try 
    {
        ghjson::Json json = ghjson::parse(jsonStr);
        for(auto &item : expect)
        {
            string key  = item.first;
            ghjson::Json value = item.second;
            cout << "expect key: " << key << ", value: " << value.dump() << ", got: "<< json[key].dump() <<endl;
            if(json[key].dump() != value.dump())
            {
                cerr << "parsing error , got: "<< json[key].dump() <<endl;
                break;
            }
        }
        cout << json.dump() << endl;
        succ++;
    } 
    catch (const ghjson::ghJsonException& ex) 
    {
        cerr << "parsing " << jsonStr
        << ", error at position " << ex.getPosition() << ": " << ex.what() << endl;
    }
    
    count++;
}

void TestparseArray(ghjson::array expect, const string jsonStr)
{
    try 
    {
        ghjson::Json json = ghjson::parse(jsonStr);
        size_t i = 0;
        for( ; i < expect.size(); i++)
        {
            string ex = expect[i].dump();
            string res = json[i].dump();
            cout << "parsing in no." << i << ", expected: " << ex << ", got: "<< res <<endl;
            if(expect[i] != json[i])
            {
                cerr << "parsing error in no." << i << ", expected: " << ex << ", got: "<< res <<endl;
                break;
            }
        }
        if(i == expect.size())
        {  
            succ++;
            cout << json.dump() << endl;
        }

    } 
    catch (const ghjson::ghJsonException& ex) 
    {
        cerr << "parsing " << jsonStr
        << ", error at position " << ex.getPosition() << ": " << ex.what() << endl;
    }
    
    count++;
}

void TestparseString(string expect, const string jsonStr)
{
    try 
    {
        ghjson::Json json = ghjson::parse(jsonStr);
        if(expect.compare(json.getString()) == 0)
            succ++;
        else
        {
            cerr << "expected : " << expect << ", got : "<<  json.getString()<< endl;
        }
    } 
    catch (const ghjson::ghJsonException& ex) 
    {
        cerr << "parsing " << jsonStr
        << ", error at position " << ex.getPosition() << ": " << ex.what() << endl;
    }
    
    count++;
}

void TestparseNumber(double num, const string jsonStr)
{
    try 
    {
        ghjson::Json json = ghjson::parse(jsonStr);
        if(fabs(num - json.getNumber()) < epsilon)
            succ++;
        else
        {
            cout << "expected : " << num << ", got : "<<  json.getNumber()<< endl;
        }
    } 
    catch (const ghjson::ghJsonException& ex) 
    {
        cerr << "parsing " << jsonStr
        << ", error at position " << ex.getPosition() << ": " << ex.what() << endl;
    }
    catch(...)
    {
        cerr << "parsing error in " << jsonStr << endl;
    }

    count++;
}

void TestparseLiteral(string jsonStr)
{
    try 
    {
        ghjson::Json json = ghjson::parse(jsonStr);
        succ++;
    } 
    catch (const ghjson::ghJsonException& ex) 
    {
        cerr << "parsing " << jsonStr
        << ",error at position " << ex.getPosition() << ": " << ex.what() << endl;
    }
    count++;
}

void TestparseWrong() 
{
    TestparseLiteral("");
    TestparseLiteral(" ");

    TestparseNumber(1., "1.");
}

void TestObject()
{
    vector<ghjson::Json> arr;
    arr.push_back(nullptr);
    arr.push_back(true);
    arr.push_back(false);
    arr.push_back(12321);

    map<string, ghjson::Json> obj;
    obj.insert(pair<string, ghjson::Json>("key1", "value1" ));
    obj.insert(pair<string, ghjson::Json>("key2", true ));
    obj.insert(pair<string, ghjson::Json>("key3", arr ));
    obj.emplace( "key4", obj);
    TestparseObject(obj, "{ \"key1\":\"value1\" , \"key2\": true , \"key3\":[ null , true , false , 12321] , \"key4\" :{ \"key1\":\"value1\" , \"key2\": true , \"key3\":[ null , true , false , 12321] }}");
}

void TestArray()
{
    cout << __func__<<endl;
    vector<ghjson::Json> arr;
    arr.push_back(nullptr);
    arr.push_back(true);
    arr.push_back(false);
    arr.push_back(12321);

    ghjson::Json a = ghjson::parse("\"Hello\"");
    arr.push_back(a);

    TestparseArray(arr, "[    null ,   true , false , 12321 , \"Hello\" ]");

}

void TestString()
{
    TestparseString("", "\"\"");
    TestparseString("Hello", "\"Hello\"");
    TestparseString("Hello\nWorld", "\"Hello\\nWorld\"");
    
    TestparseString("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    //TestparseString("Hello\0World", "\"Hello\\u0000World\"");
    //TestparseString("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    //TestparseString("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    //TestparseString("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    //TestparseString("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
    //TestparseString("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}

void TestNumber()
{
    TestparseNumber(0.0, "0");
    TestparseNumber(0.0, "-0");
    TestparseNumber(0.0, "-0.0");
    TestparseNumber(1.0, "1");
    TestparseNumber(-1.0, "-1");
    TestparseNumber(1.5, "1.5");
    TestparseNumber(-1.5, "-1.5");
    TestparseNumber(3.1416, "3.1416");
    TestparseNumber(1E10, "1E10");
    TestparseNumber(1e10, "1e10");
    TestparseNumber(1E+10, "1E+10");
    TestparseNumber(1E-10, "1E-10");
    TestparseNumber(-1E10, "-1E10");
    TestparseNumber(-1e10, "-1e10");
    TestparseNumber(-1E+10, "-1E+10");
    TestparseNumber(-1E-10, "-1E-10");
    TestparseNumber(1.234E+10, "1.234E+10");
    TestparseNumber(1.234E-10, "1.234E-10");
    TestparseNumber(0.0, "1e-10000"); /* must underflow */

    TestparseNumber(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TestparseNumber( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TestparseNumber(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TestparseNumber( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TestparseNumber(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TestparseNumber( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TestparseNumber(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TestparseNumber( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TestparseNumber(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

void TestLiteral()
{
    TestparseLiteral("true");
    TestparseLiteral("false");
    TestparseLiteral("null");
}

void Testparse()
{
    TestLiteral();
    TestNumber();
    TestString();
    TestArray();
    TestObject();

    //TestparseWrong();
}

void TestSetNumer()
{
    ghjson::Json num ={1234};
    cout << "Before: "<< num.getNumber() << endl;
    num.setNumber(99.123);
    cout << "After: "<< num.getNumber() << endl;
}

void TestSetArray()
{
    ghjson::array test = {}; 
    ghjson::Json jsonarray (test);

    jsonarray.addToArray({});
    jsonarray.addToArray(true);
    jsonarray.addToArray(123123);
    jsonarray.addToArray("12312312312312312123");
    cout << "before:\n"<< jsonarray.dump() << endl;
    for(auto iter = jsonarray.arrayBegin(); iter < jsonarray.arrayEnd(); iter++)
    {
        if(iter->is_number() && iter->getNumber() == 123123)
        {
            cout << "find!" << endl;
            iter->setNumber(98765);
        }
    }
    cout << "after:\n"<< jsonarray.dump() << endl;
}

void TestSetObject()
{
    ghjson::object test = {}; 
    ghjson::Json jsonobject(test);
    cout << jsonobject.type() << endl;
    jsonobject.addToObject("key1", 123);
    jsonobject.addToObject("key2", true);
    jsonobject.addToObject("key213213", "12312312");
    cout << "before:\n"<< jsonobject.dump() << endl;
    for(auto iter = jsonobject.objectBegin(); iter != jsonobject.objectEnd(); iter++)
    {
        if ( iter->second.is_number() && iter->second.getNumber() == 123)
            iter->second.setNumber(987);
        else if ( iter->second.is_string() )
            iter->second.setString("success");
    }
    for(auto iter = jsonobject.objectBegin_const(); iter != jsonobject.objectEnd_const(); iter++)
    {
        cout << "const dump : " << iter->first << " : "<<  iter->second.dump()<< endl;
    }
    cout << "after:\n"<< jsonobject.dump() << endl;
}

void TestSet()
{
    TestSetNumer();
    TestSetArray();
    TestSetObject();
    
}

void TestOther()
{
    ghjson::Json test1;
    cout << test1.getNumber() << endl;
    cout << test1.type() << endl;
    test1.setNumber(12) ;
    cout << test1.getNumber() << endl;
}

int main()
{
    //Testparse();
    //cout << "success :" << succ << " total :" << count << endl;
    //TestOther();
    TestSet();
}