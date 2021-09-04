//
// Created by eava_wu on 18-10-2.
//

#include <jni.h>
#include "include/Hello.h"
#include<android/log.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>

#define TAG "helloworld-jni" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型

#ifdef __cplusplus
extern "C" {
#endif

static sigset_t xcc_signal_trace_oldset;

static void xc_trace_handler(int sig, siginfo_t *si, void *uc) {
    LOGD("xc_trace_handler sig = %d",sig);
}

int xcc_signal_trace_register(void (*handler)(int, siginfo_t *, void *)) {
    int r;
    sigset_t set;
    struct sigaction act;
    //un-block the SIGQUIT mask for current thread, hope this is the main thread
    //重置set,等待使用
    sigemptyset(&set);
    //添加信号到信号集合
    sigaddset(&set, SIGQUIT);

    LOGD("pthread_sigmask before");
    //SIG_UNBLOCK 清空内存中的，据用户设置的数据，对内核中的数据进行解除阻塞，就是解除内核中set的最,失败直接返回errorNo
    if (0 != (r = pthread_sigmask(SIG_UNBLOCK, &set, NULL))) return r;
    //register new signal handler for SIGQUIT(注册新的SIGQUIT捕获函数)
    memset(&act, 0, sizeof(act));
    sigfillset(&act.sa_mask);
    act.sa_sigaction = handler;
    act.sa_flags = SA_RESTART | SA_SIGINFO;
    LOGD("pthread_sigmask after");
    if (0 != sigaction(SIGQUIT, &act, NULL)) {
        //注册SIG_QUIT失败，清空内核中的sigaction
        pthread_sigmask(SIG_SETMASK, &xcc_signal_trace_oldset, NULL);
        return 1;
    }
    return 0;
}

void registerSig() {
    LOGD("registerSig");
    xcc_signal_trace_register(xc_trace_handler);
}

//回调函数 在这里面注册函数
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    //env赋值
    LOGD("GetEnv %d", getpid());
    //获取JniEnv
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        LOGD("GetEnv Failed");
        return -1;
    }
    registerSig();
    //返回jni 的版本
    return JNI_VERSION_1_6;
}
#ifdef __cplusplus
}
#endif
