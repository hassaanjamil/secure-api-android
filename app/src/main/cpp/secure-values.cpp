#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_app_sample_secureapi_SecureKeyLibrary_getAPIKey(JNIEnv* env, jobject /* this */, jint flavor) {
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