#include <iostream>
#include <cmath>
#include "ghjson.hpp"

using namespace std;
int succ = 0;
int count  = 0;
double epsilon = 1e-5;

void TestParseArray(ghjson::array expect, string jsonStr)
{
    try 
    {
        ghjson::Json json = ghjson::Parse(jsonStr);
        for(size_t i ; i < expect.size(); i++)
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
    } 
    catch (const ghjson::JsonParseException& ex) 
    {
        cerr << "parsing " << jsonStr
        << ", error at position " << ex.GetPosition() << ": " << ex.what() << endl;
    }
    
    count++;
}

void TestParseString(string expect, string jsonStr)
{
    try 
    {
        ghjson::Json json = ghjson::Parse(jsonStr);
        if(expect.compare(json.GetString()) == 0)
            succ++;
        else
        {
            cout << "expected : " << expect << ", got : "<<  json.GetString()<< endl;
        }
    } 
    catch (const ghjson::JsonParseException& ex) 
    {
        cerr << "parsing " << jsonStr
        << ", error at position " << ex.GetPosition() << ": " << ex.what() << endl;
    }
    
    count++;
}

void TestParseNumber(double num, string jsonStr)
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
    catch (const ghjson::JsonParseException& ex) 
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
    catch (const ghjson::JsonParseException& ex) 
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

void TestArray()
{
    ghjson::Json expected ={["null", "false" , "true" , 123 , "\"abc\"" ]};
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
    TestLiteral();
    TestNumber();
    TestString();

    //TestParseWrong();
}

int main()
{
    TestParse();
    cout << "success :" << succ << " total :" << count << endl;
}