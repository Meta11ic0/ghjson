#include <iostream>
#include <cmath>
#include "ghjson.hpp"

using namespace std;
int succ = 0;
int count  = 0;
double epsilon = 1e-5;

void TestParseObject(ghjson::object expect, const string jsonStr)
{
    cout << __func__<<endl;
    try 
    {
        ghjson::Json json = ghjson::Parse(jsonStr);
        for(auto &item : expect)
        {
            string key  = item.first;
            ghjson::Json value = item.second;
            cout << "expect key: " << key << ", value: " << value.Dump() << ", got: "<< json[key].Dump() <<endl;
            if(json[key].Dump() != value.Dump())
            {
                cerr << "parsing error , got: "<< json[key].Dump() <<endl;
                break;
            }
        }
        string ss = json.Dump();
        cout << ss << endl;
        succ++;
    } 
    catch (const ghjson::ghJsonException& ex) 
    {
        cerr << "parsing " << jsonStr
        << ", error at position " << ex.GetPosition() << ": " << ex.what() << endl;
    }
    
    count++;
}

void TestParseArray(ghjson::array expect, const string jsonStr)
{
    try 
    {
        ghjson::Json json = ghjson::Parse(jsonStr);
        size_t i = 0;
        for( ; i < expect.size(); i++)
        {
            string ex, res;
            expect[i].Dump(ex);
            json[i].Dump(res);
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
        << ", error at position " << ex.GetPosition() << ": " << ex.what() << endl;
    }
    
    count++;
}

void TestParseString(string expect, const string jsonStr)
{
    try 
    {
        ghjson::Json json = ghjson::Parse(jsonStr);
        if(expect.compare(json.GetString()) == 0)
            succ++;
        else
        {
            cerr << "expected : " << expect << ", got : "<<  json.GetString()<< endl;
        }
    } 
    catch (const ghjson::ghJsonException& ex) 
    {
        cerr << "parsing " << jsonStr
        << ", error at position " << ex.GetPosition() << ": " << ex.what() << endl;
    }
    
    count++;
}

void TestParseNumber(double num, const string jsonStr)
{
    try 
    {
        ghjson::Json json = ghjson::Parse(jsonStr);
        if(fabs(num - json.GetNumber()) < epsilon)
            succ++;
        else
        {
            cout << "expected : " << num << ", got : "<<  json.GetNumber()<< endl;
        }
    } 
    catch (const ghjson::ghJsonException& ex) 
    {
        cerr << "parsing " << jsonStr
        << ", error at position " << ex.GetPosition() << ": " << ex.what() << endl;
    }
    count++;
}

void TestParseLiteral(string jsonStr)
{
    try 
    {
        ghjson::Json json = ghjson::Parse(jsonStr);
        succ++;
    } 
    catch (const ghjson::ghJsonException& ex) 
    {
        cerr << "parsing " << jsonStr
        << ",error at position " << ex.GetPosition() << ": " << ex.what() << endl;
    }
    count++;
}

void TestParseWrong() 
{
    TestParseLiteral("");
    TestParseLiteral(" ");

    TestParseNumber(1., "1.");
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
    TestParseObject(obj, "{ \"key1\":\"value1\" , \"key2\": true , \"key3\":[ null , true , false , 12321] }");
}

void TestArray()
{
    cout << __func__<<endl;
    vector<ghjson::Json> arr;
    arr.push_back(nullptr);
    arr.push_back(true);
    arr.push_back(false);
    arr.push_back(12321);

    ghjson::Json a = ghjson::Parse("\"Hello\"");
    arr.push_back(a);
    arr.push_back(arr);
    arr.push_back(arr);

    TestParseArray(arr, "[ null , true , false , 12321 , \"Hello\" ,[ null , true , false , 12321 , \"Hello\" ], [ null , true , false , 12321 , \"Hello\" ,[ null , true , false , 12321 , \"Hello\" ]]]");

}

void TestString()
{
    TestParseString("", "\"\"");
    TestParseString("Hello", "\"Hello\"");
    TestParseString("Hello\nWorld", "\"Hello\\nWorld\"");
    
    TestParseString("\" \\ / \b \f \n \r \t", "\"\\\" \\\\ \\/ \\b \\f \\n \\r \\t\"");
    //TestParseString("Hello\0World", "\"Hello\\u0000World\"");
    //TestParseString("\x24", "\"\\u0024\"");         /* Dollar sign U+0024 */
    //TestParseString("\xC2\xA2", "\"\\u00A2\"");     /* Cents sign U+00A2 */
    //TestParseString("\xE2\x82\xAC", "\"\\u20AC\""); /* Euro sign U+20AC */
    //TestParseString("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");  /* G clef sign U+1D11E */
    //TestParseString("\xF0\x9D\x84\x9E", "\"\\ud834\\udd1e\"");  /* G clef sign U+1D11E */
}

void TestNumber()
{
    TestParseNumber(0.0, "0");
    TestParseNumber(0.0, "-0");
    TestParseNumber(0.0, "-0.0");
    TestParseNumber(1.0, "1");
    TestParseNumber(-1.0, "-1");
    TestParseNumber(1.5, "1.5");
    TestParseNumber(-1.5, "-1.5");
    TestParseNumber(3.1416, "3.1416");
    TestParseNumber(1E10, "1E10");
    TestParseNumber(1e10, "1e10");
    TestParseNumber(1E+10, "1E+10");
    TestParseNumber(1E-10, "1E-10");
    TestParseNumber(-1E10, "-1E10");
    TestParseNumber(-1e10, "-1e10");
    TestParseNumber(-1E+10, "-1E+10");
    TestParseNumber(-1E-10, "-1E-10");
    TestParseNumber(1.234E+10, "1.234E+10");
    TestParseNumber(1.234E-10, "1.234E-10");
    TestParseNumber(0.0, "1e-10000"); /* must underflow */

    TestParseNumber(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
    TestParseNumber( 4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
    TestParseNumber(-4.9406564584124654e-324, "-4.9406564584124654e-324");
    TestParseNumber( 2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
    TestParseNumber(-2.2250738585072009e-308, "-2.2250738585072009e-308");
    TestParseNumber( 2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
    TestParseNumber(-2.2250738585072014e-308, "-2.2250738585072014e-308");
    TestParseNumber( 1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
    TestParseNumber(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}

void TestLiteral()
{
    TestParseLiteral("true");
    TestParseLiteral("false");
    TestParseLiteral("null");
}

void TestParse()
{
    //TestLiteral();
    //TestNumber();
    //TestString();
    //TestArray();
    TestObject();

    //TestParseWrong();
}

void TestSetNumer()
{
    ghjson::Json num ={1234};
    cout << num.GetNumber() << endl;
    cout << num.Type() << endl;
    num.SetNumber(99.123);
    cout << num.GetNumber() << endl;
}

void TestSetArray()
{
    cout << __func__ << endl;
    ghjson::array test = {}; 
    ghjson::Json jsonarray (test);

    jsonarray.AddToArray({});
    jsonarray.AddToArray(true);
    jsonarray.AddToArray(123123);
    jsonarray.AddToArray("12312312312312312123");
    cout << jsonarray.GetArray().size() << endl;
    cout << jsonarray.Dump() << endl;
    for(auto iter  : jsonarray.GetArray())
    {
        if(iter.is_number() && iter.GetNumber() == 123123)
        {
            cout << "Before modification: " << iter.GetNumber() << endl;
            iter.SetNumber(1234);
            cout << "After modification: " << iter.GetNumber() << endl;
        }
    }
    cout << jsonarray.Dump() << endl;
}

void TestSetObject()
{
    ghjson::object test = {}; 
    ghjson::Json jsonboject(test);
    cout << jsonboject.Type() << endl;
    jsonboject.AddToObject("key1", 123);
    jsonboject.AddToObject("key2", true);
    jsonboject.AddToObject("key213213", "12312312");
    cout << jsonboject.Dump() << endl;
    ghjson::object tt = jsonboject.GetObject();
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
    cout << test1.GetNumber() << endl;
    cout << test1.Type() << endl;
    test1.SetNumber(12) ;
    cout << test1.GetNumber() << endl;
}

int main()
{
    //TestParse();
    //cout << "success :" << succ << " total :" << count << endl;
    //TestOther();
    TestSet();
}