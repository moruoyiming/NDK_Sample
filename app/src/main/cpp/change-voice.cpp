#include <unistd.h>
#include "com_example_ndk_ChangeVoiceActivity.h"

using namespace FMOD;

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndk_ChangeVoiceActivity_voiceChangeNative
        (JNIEnv *env, jobject thiz, jint mode, jstring path) {

    char *content_ = "默认 播放完毕";
    //C 认识的字符串
    const char *path_ = env->GetStringUTFChars(path, NULL);

    // 音效引擎系统 FMOD::
    System *system = 0;
    //声音 指针
    Sound *sound = 0;
    //通道 音轨 声音在上面跑，跑道指针
    Channel *channel = 0;
//    DSP digital signal process=数字信号处理
    DSP *dsp = 0;

//    Java思想初始化
//    system = xxxx();
//    C思想初始化
//    xxxx(&system)

//   TODO 第一步 创建系统
    System_Create(&system);
//   TODO 第二步 系统的初始化 参数1：最大音轨数 参数2：初始化标记 参数3：
    system->init(32, FMOD_INIT_NORMAL, 0);
//   TODO 第三步 创建声音  参数1：路径 参数2：声音初始化标记 参数3：额外数据 参数4：声音指针
    system->createSound(path_, FMOD_DEFAULT, 0, &sound);
//   TODO 第四步 播放声音  参数1：声音 参数2：分组音轨 参数3：控制 参数4：音轨
    system->playSound(sound, 0, false, &channel);

//   TODO 第五步 增加特效
    switch (mode) {
        case com_example_ndk_ChangeVoiceActivity_MODE_NORMAL:
            content_ = "原声 播放完毕";
            break;
        case com_example_ndk_ChangeVoiceActivity_MODE_LUOLI:
            content_ = "萝莉 播放完毕";
            //音调高  萝莉2.0
            //1.创建DSP类型的Pitch音调调节
            system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
            //2.设置Pitch音调调节2.0
            dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 2.0f);
            //3.添加音效进去 音轨
            channel->addDSP(0, dsp);
            break;
        case com_example_ndk_ChangeVoiceActivity_MODE_DASHU:
            content_ = "大叔 播放完毕";
            //1.创建DSP类型的Pitch音调调节
            system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
            //2.设置Pitch音调调节2.0
            dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.8f);
            //3.添加音效进去 音轨
            channel->addDSP(0, dsp);
            break;
        case com_example_ndk_ChangeVoiceActivity_MODE_GAOGUAI:
            content_ = "搞怪 播放完毕";
            //修改声音频率
            float mFrequency;
            channel->getFrequency(&mFrequency);
            //修改频率
            channel->setFrequency(mFrequency * 1.5f);
            break;
        case com_example_ndk_ChangeVoiceActivity_MODE_JINGSONG:
            content_ = "惊悚 播放完毕";

            //1.创建DSP类型的Pitch音调调节
            system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
            //2.设置Pitch音调调节2.0
            dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.8f);
            //3.添加音效进去 音轨
            channel->addDSP(0, dsp);

            system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
            dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 200);
            dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 20);
            channel->addDSP(1, dsp);

            system->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &dsp);
            dsp->setParameterFloat(FMOD_DSP_TREMOLO_FREQUENCY, 20);
            dsp->setParameterFloat(FMOD_DSP_TREMOLO_SKEW, 0.8f);
            channel->addDSP(2, dsp);
            break;
        case com_example_ndk_ChangeVoiceActivity_MODE_KONGLING:
            content_ = "空灵 播放完毕";
            // 回音
            system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
            // 回音延时度
            dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 200);
            dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 20);
            channel->addDSP(0, dsp);
            break;
    }

    //等待播放完毕 再回收
    bool isPlayer = true;
    while (isPlayer) {
        channel->isPlaying(&isPlayer);//如果播放完毕，音轨是知道，内部修改isPlayer =false;
        usleep(1000 * 1000);
    }

    sound->release();
    system->close();
    system->release();
    env->ReleaseStringUTFChars(path, path_);

}