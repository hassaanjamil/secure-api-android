# Secure API Key using CMake and by defining flavors in Gradle Module

### Prerequisite:

- **Android Studio** with **Android SDK**
- Install SDK Components from SDK Manager: **NDK, CMake, LLDB**

### Steps to achieve:

1. Create new *Native C++ Project* from *Android Studio* from **New->New Project->Native C++**

2. You will see files *native-lib.cpp* in module **app->src->main->cpp**, leave it as is for now.

3. Go into you app package in **app->src->main->java** and create a Kotlin/Java class/object in it and copy paste the following code snippet in that class/object, lets take it as a Kotlin Object.

   ```kotlin
   init { System.loadLibrary("native-lib") }
   
   external fun getAPIKey(flavor:Int): String
   ```

4. You can call the function `getAPIKey()` in your `MainActivity` as

   ```kotlin
   // Example of a call to a native method
   binding.sampleText.text = SecureKeyLibrary.getAPIKey(BuildConfig.flavor)
   ```

5. Now you need to add the following code snippet in your **native-lib.cpp** file

   ```cpp
   #include <jni.h>
   #include <string>
   
   extern "C" JNIEXPORT jstring JNICALL
       
   Java_<PACKAGE_NAME>_<CLASS_NAME>_getAPIKey(JNIEnv* env, jobject /* this */, jint flavor) {
       std::string api_key = "";
       std::string api_key_p = "<APIKEY-PRODUCTION>";
       std::string api_key_s = "<APIKEY-STAGING>";
       std::string api_key_t = "<APIKEY-TESTING>";
       switch (flavor) {
           case 1:
               api_key = api_key_p;
               break;
           case 2:
               api_key = api_key_s;
               break;
           case 3:
               api_key = api_key_t;
               break;
       }
       return env->NewStringUTF(api_key.c_str());
   }
   ```

6. Copy and paste the following code snippet in **src->main->cpp->CMakeLists.txt** file

   ```c++
   # For more information about using CMake with Android Studio, read the
   # documentation: https://d.android.com/studio/projects/add-native-code.html
   
   # Sets the minimum version of CMake required to build the native library.
   
   cmake_minimum_required(VERSION 3.10.2)
   
   # Creates and names a library, sets it as either STATIC
   # or SHARED, and provides the relative paths to its source code.
   # You can define multiple libraries, and CMake builds them for you.
   # Gradle automatically packages shared libraries with your APK.
   
   add_library( # Sets the name of the library.
           secure-values
   
           # Sets the library as a shared library.
           SHARED
   
           # Provides a relative path to your source file(s).
           native-lib.cpp)
   
   # Searches for a specified prebuilt library and stores the path as a
   # variable. Because CMake includes system libraries in the search path by
   # default, you only need to specify the name of the public NDK library
   # you want to add. CMake verifies that the library exists before
   # completing its build.
   
   find_library( # Sets the name of the path variable.
           log-lib
   
           # Specifies the name of the NDK library that
           # you want CMake to locate.
           log )
   
   # Specifies libraries CMake should link to your target library. You
   # can link multiple libraries, such as libraries you define in this
   # build script, prebuilt third-party libraries, or system libraries.
   
   target_link_libraries( # Specifies the target library.
           secure-values
   
           # Links the target library to the log library
           # included in the NDK.
           ${log-lib} )
   ```

7. Add the following configuration in your **app** module's **build.gradle** file

   ```groovy
   android {
       ...
       externalNativeBuild {
           cmake {
               path file('src/main/cpp/CMakeLists.txt')
               version '3.10.2'
           }
       }
       ...
       productFlavors {
           ...
           staging {
               // Assigns this product flavor to the "version" flavor dimension.
               // If you are using only one dimension, this property is optional,
               // and the plugin automatically assigns all the module's flavors to
               // that dimension.
               dimension "version"
               buildConfigField "int", "flavor", "2"
           }
           production {
               dimension "version"
               buildConfigField "int", "flavor", "1"
           }
           qa {
               dimension "version"
               buildConfigField "int", "flavor", "3"
           }
           ...
       }
       ...
   }
   ```

8. Open **View->Tool Windows->Build Variants**, select a variant from *Active Build Variant* field, for now, lets say select *stagingDebug* from variants drop down.

9. Select **Build->Rebuild Project** so that your Module's `BuildConfig.java` generated, will contain the generated code like this in ***app->build->generated->source->buildingConfig->staging->debug->your-package->BuildConfig.java***.

   ```java
   package com.app.sample.secureapi;
   
   public final class BuildConfig {
     public static final boolean DEBUG = Boolean.parseBoolean("true");
     public static final String APPLICATION_ID = "com.app.sample.secureapi";
     public static final String BUILD_TYPE = "debug";
     public static final String FLAVOR = "staging";
     public static final int VERSION_CODE = 1;
     public static final String VERSION_NAME = "1.0";
     // Field from product flavor: staging
     public static final int flavor = 2;
   }
   ```
   
10. You can see flavor variable generated above as you defined flavor in Gradle configurations.
    Then you need to create a class (Java/Kotlin) in your app module under your package e.g. `SecureKeyLibrary`, and now we will utilize the **flavor** value of **BuildConfig.java** file in our **native-lib.cpp**, and it will be as:

    ```cpp
    #include <jni.h>
    #include <string>
    
    extern "C" JNIEXPORT jstring JNICALL
    // For Example:
    // Java_com_app_sample_secureapi_SecureKeyLibrary_getAPIKey
    Java_<class-package-name>_<java-kotlin-class-name>_<function-name>(JNIEnv* env, jobject /* this */, jint flavor) {
        std::string api_key = "";
        std::string api_key_p = "<APIKEY-PRODUCTION>";
        std::string api_key_s = "<APIKEY-STAGING>";
        std::string api_key_t = "<APIKEY-TESTING>";
        switch (flavor) {
            case 1:
                api_key = api_key_p;
                break;
            case 2:
                api_key = api_key_s;
                break;
            case 3:
                api_key = api_key_t;
                break;
        }
        return env->NewStringUTF(api_key.c_str());
    }
    ```

    >You can place your keys as it is or after applying some encoding techniques to make it more harder to reverse. 

11. And you kotlin/Java class/object definition will be as e.g. as `SecureKeyLibrary` as mentioned below:

    ```kotlin
    package com.app.sample.secureapi
    
    object SecureKeyLibrary {
        init {
            System.loadLibrary("native-lib")
        }
    
        external fun getAPIKey(flavor:Int): String
    }
    ```

12. And in your activity you will call the above function `getAPIKey()` by passing **flavor** value to its parameter so that it can return you **API Key** for selected build variant:

    ```kotlin
    ackage com.app.sample.secureapi
    
    import androidx.appcompat.app.AppCompatActivity
    import android.os.Bundle
    import com.app.sample.secureapi.databinding.ActivityMainBinding
    
    class MainActivity : AppCompatActivity() {
    
        private lateinit var binding: ActivityMainBinding
    
        override fun onCreate(savedInstanceState: Bundle?) {
            super.onCreate(savedInstanceState)
    
            binding = ActivityMainBinding.inflate(layoutInflater)
            setContentView(binding.root)
    
            // Example of a call to a native method
            binding.sampleText.text = SecureKeyLibrary.getAPIKey(BuildConfig.flavor)
        }
    }
    ```

    >  Select **Build -> Make Project** to build library's `*.so` files for abis/multiple architectures. Always use **Build -> Clean Project and Rebuild Project** to initiate fresh rebuilding after changing the active build variant.

    > **Note: **You can place your keys as it is or after applying some encoding techniques to make it more harder to reverse. 

    > **Use: **After selecting build variant and you will get the build variant key, you can switch this implementation for having different secure base urls with their endpoints for different build variants and for API Keys as above.  

    Reference: https://blog.mindorks.com/securing-api-keys-using-android-ndk