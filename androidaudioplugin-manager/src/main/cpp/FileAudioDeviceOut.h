//
// Created by leoni on 18.05.2025.
//

#ifndef AAP_CORE_FILEAUDIODEVICEOUT_H
#define AAP_CORE_FILEAUDIODEVICEOUT_H

#include "AudioDevice.h"
#include <audio/choc_SampleBuffers.h>
#include <fcntl.h>    // open
#include <unistd.h>   // write, close
#include <jni.h>

namespace aap {

    class FileAudioDeviceOut : public AudioDeviceOut {

        JNIEnv *env;

        void startCallback() override {
            // todo: instruct to create a file and prepare to write data to it
        }

        void stopCallback() override {
            // todo: instruct to flush buffer (if i implement a buffer, as i should)
            // and to save the file
        }

        // todo: write to file descriptor passed from kotlin?
        // https://developer.android.com/ndk/reference/group/file-descriptor
        void write(aap::AudioBuffer *audioDataToWrite, int32_t bufferPosition, int32_t numFrames) override {
            // todo: do the interleaving myslef, bc this is non-interleaved data apparently

            // here's an example of how interleaving is done to feed data to Oboe

            // auto oboeView = choc::buffer::createInterleavedView((float*) oboeAudioData, audioStream->getChannelCount(), numFrames);
            //        choc::buffer::copy(oboeView, aap_buffer.audio.getStart(numFrames));

            // This puts `srcAudioData` into current AAP output buffer (ring buffer).
            // FIXME: currentPosition?
            choc::buffer::FrameRange range{0, (uint32_t ) numFrames};
            auto bufferView = audioDataToWrite->audio.getView().getFrameRange(range);

            writeCHOCBufferAsRawPCM(bufferView, numFrames);
            //choc::buffer::copy(aap_buffer.audio.getFrameRange(range), audioDataToWrite->audio.getView().getFrameRange(range));

            // todo: copy to file here
        }

        void writeCHOCBufferAsRawPCM(
                const choc::buffer::BufferView<float, choc::buffer::SeparateChannelLayout>& buffer,
                int32_t numFrames)
        {

            for (size_t frame = 0; frame < numFrames; ++frame)
            {
                for (size_t ch = 0; ch < buffer.getNumChannels(); ++ch)
                {
                    float sample = buffer.getSample(ch, frame);
                    writeSample(sample);
                    //file.write(reinterpret_cast<const char*>(&sample), sizeof(float));
                }
            }

            // file.close();
        }

        void writeSample(float sample) {
            auto clazz = env->FindClass("org/androidaudioplugin/manager/PluginPlayer");
            auto method = env->GetStaticMethodID(clazz, "callback", "(F)V");
            env->CallStaticVoidMethod(clazz, method, sample);
        };

    };





} // aap

#endif //AAP_CORE_FILEAUDIODEVICEOUT_H
