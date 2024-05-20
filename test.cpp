#include <iostream>
#include <cmath>
#include "ghjson.hpp"

using namespace std;
int succ = 0;
int count  = 0;
double epsilon = 1e-5;

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

void TestLiteral()
{
    TestParseLiteral("true");
    TestParseLiteral("false");
    TestParseLiteral("null");
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

void TestParse()
{
    TestLiteral();
    TestNumber();

    TestParseWrong();
}

int main()
{
    TestParse();
    cout << "success :" << succ << " total :" << count << endl;
}