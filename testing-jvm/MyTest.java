//===================================================================================================================|
// A little java class to test with jni
//
// By:
//  Aethiopis II ben Zahab
//
// Date Created:
//  15th of January 2021, Saturday.
//===================================================================================================================|
public class MyTest {

    private static int magic_counter=777;       // a jackpot

    public static void MyMain() // we call this method from C++; using JNI, here I come neo4j
    {
        System.out.println("At last we will reavel ourselves to the Jedi, at last we will have regvenge!!!");
        System.out.println(magic_counter);
    } // end MyMain
} // end MyTest
        
