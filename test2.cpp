#include <iostream>
#include <cmath>
#include "ghjson.hpp"

using namespace std;
int succ = 0;
int count  = 0;
double epsilon = 1e-5;

void TestparseObject(ghjson::object expect, const string jsonStr)
{
    cout << __func__<<endl;
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
        string ss = json.dump();
        cout << ss << endl;
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
            string ex, res;
            expect[i].dump(ex);
            json[i].dump(res);
            cout << "parsing in no." << i << ", expected: " << ex << ", got: "<< res <<endl;
            if(expect[i] != json[i])
            {
                cerr << "parsing error in no." << i << ", expected: " << ex << ", got: "<< res <<endl;
                break;
            }
        }
        if(i == expect.size())
            succ++;
        
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
    cout << __func__<<endl;
    vector<ghjson::Json> arr;
    arr.push_back(nullptr);
    arr.push_back(true);
    arr.push_back(false);
    arr.push_back(12321);

    map<string, ghjson::Json> obj;
    obj.insert(pair<string, ghjson::Json>("key1", "value1" ));
    obj.insert(pair<string, ghjson::Json>("key2", true ));
    obj.insert(pair<string, ghjson::Json>("key3", arr ));
    TestparseObject(obj, "{ \"key1\":\"value1\" , \"key2\": true , \"key3\":[ null , true , false , 12321] }");
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
    arr.push_back(arr);
    arr.push_back(arr);

    TestparseArray(arr, "[ null , true , false , 12321 , \"Hello\" ,[ null , true , false , 12321 , \"Hello\" ], [ null , true , false , 12321 , \"Hello\" ,[ null , true , false , 12321 , \"Hello\" ]]]");

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
    //TestLiteral();
    //TestNumber();
    //TestString();
    //TestArray();
    TestObject();

    //TestparseWrong();
}

void TestSetNumer()
{
    ghjson::Json num ={1234};
    cout << num.getNumber() << endl;
    cout << num.type() << endl;
    num.setNumber(99.123);
    cout << num.getNumber() << endl;
}

void TestSetArray()
{
    cout << __func__ << endl;
    ghjson::array test = {}; 
    ghjson::Json jsonarray (test);

    jsonarray.addToArray({});
    jsonarray.addToArray(true);
    jsonarray.addToArray(123123);
    jsonarray.addToArray("12312312312312312123");
    cout << jsonarray.getArray().size() << endl;
    cout << jsonarray.dump() << endl;
    for(auto iter  : jsonarray.getArray())
    {
        if(iter.is_number() && iter.getNumber() == 123123)
        {
            cout << "Before modification: " << iter.getNumber() << endl;
            iter.setNumber(1234);
            cout << "After modification: " << iter.getNumber() << endl;
        }
    }
    cout << jsonarray.dump() << endl;
}

void TestSetObject()
{
    ghjson::object test = {}; 
    ghjson::Json jsonboject(test);
    cout << jsonboject.type() << endl;
    jsonboject.addToObject("key1", 123);
    jsonboject.addToObject("key2", true);
    jsonboject.addToObject("key213213", "12312312");
    cout << jsonboject.dump() << endl;
    ghjson::object tt = jsonboject.getObject();
    for(auto item : test)
    {
        cout << item.first  << endl;
    }
}

void TestSet()
{
    TestSetNumer();
    TestSetArray();
    //TestSetObject();
    
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