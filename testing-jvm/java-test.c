//===================================================================================================================|
// Testing the Java Virtual Machine with JNI; aka intro to JNI.
//
// By:
//  Aethiopis II ben Zahab
//
// Date Created:
//  15th of January 2021, Saturday.
//
//g++ -g -I/usr/lib/jvm/java-11-openjdk-amd64/include 
//  -I/usr/lib/jvm/java-11-openjdk-amd64/include/linux 
//  -L/usr/lib/jvm/java-11-openjdk-amd64/lib/ 
//  -L/usr/lib/jvm/java-11-openjdk-amd64/lib/server/ 
//  -L/usr/lib -ljvm java-test.c
//===================================================================================================================|
#include <jni.h>     
#include <iostream>
using namespace std;

#include <string.h>

int main()
{
    JavaVM *jvm;        // pointer to JVM 
    JNIEnv *env;        // pointer to native interface
    
    JavaVMInitArgs vm_args;                             // intialization arguments
    JavaVMOption *options = new JavaVMOption[1];        // JVM invocation options
    options[0].optionString = const_cast<char *>("-Djava.class.path=.");    // where to find my java .class
    vm_args.version = JNI_VERSION_1_6;                  // minimum Java version
    vm_args.nOptions = 1;                               // number of options
    vm_args.options = options;
    vm_args.ignoreUnrecognized = false;                 // invalid options make the JVM init
    
    // load and initalize Java VM and JNI interface
    jint rc = JNI_CreateJavaVM(&jvm, (void **)&env, &vm_args);
    delete options;     // we then no longer need the initialization options.
    if (rc != JNI_OK) {
        // TO DO: error processing ...
        cin.get();
        exit(EXIT_FAILURE);
    } // end if
    
    cout << "JVM load succeeded: Version ";
    jint ver = env->GetVersion();
    cout << ((ver>>16)&0x0f) << "." << (ver & 0x0f) << endl;
    
    //  add code that'll use jvm here
    jclass cls = env->FindClass("MyTest");      // try to find the class
    if (cls == nullptr)
    {
        cerr << "err: class not found";
    } // end if
    else
    {
        cout << "class found\nInvoking Java method\n";
        jmethodID mid = env->GetStaticMethodID(cls, "MyMain", "()V");
        if (mid == nullptr)
            cerr << "MyMain method not found.";
        else
        {
            env->CallStaticVoidMethod(cls, mid);
            cout << endl;
        } // end else
    } // end else
    
    jvm->DestroyJavaVM();
    cin.get();
} // end main

//===================================================================================================================|
//          THE END
//===================================================================================================================|
