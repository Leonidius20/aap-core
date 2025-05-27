//
// Created by leoni on 18.05.2025.
//

#ifndef AAP_CORE_FILEAUDIODEVICEOUT_H
#define AAP_CORE_FILEAUDIODEVICEOUT_H

#include "AudioDevice.h"
#include "fdstream.h"
#include <audio/choc_SampleBuffers.h>
#include <fcntl.h>    // open
#include <unistd.h>   // write, close
#include <jni.h>
#include <audio/choc_AudioFileFormat_WAV.h>
#include <android/log.h>

#define LOG_TAG "FileAudioDeviceOut"

namespace aap {

    class FileAudioDeviceOut : public AudioDeviceOut {

    protected:
        void* callback_context;
        AudioDeviceCallback *aap_callback;
        std::unique_ptr<choc::audio::AudioFileWriter> fileWriter;
        AudioBuffer aap_buffer;
        uint32_t framesPerCallback;
        // todo: num of frames to write

    public:
        FileAudioDeviceOut(uint32_t sampleRate, uint32_t framesPerCallback, int32_t numChannels, int outputFileDescriptor)
            : aap_buffer(numChannels, (int32_t) framesPerCallback), framesPerCallback(framesPerCallback) {

            if (outputFileDescriptor == -1) {
                this->fileWriter = std::unique_ptr<choc::audio::AudioFileWriter>(nullptr);
                __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, "Invalid out file descriptor (-1)");
            } else {
                auto stream = createOstreamFromFd(outputFileDescriptor);
                choc::audio::WAVAudioFileFormat<true> formatWav{};
                choc::audio::AudioFileProperties props {
                        "wav",
                        static_cast<double>(sampleRate),
                        0, // todo: use same as input. This is why wav file is incorrect
                        static_cast<uint32_t>(numChannels),
                };

                this->fileWriter = formatWav.createWriter(stream, props);
            }


            // todo: we should probably also create some sort of buffer here sized
            // framesPerCallback????
        }

        ~FileAudioDeviceOut() {
            aap_callback = nullptr;
        }

        void setAudioCallback(
                aap::AudioDeviceCallback *audioDeviceCallback,
                void *callbackContext) override {
            aap_callback = audioDeviceCallback;
            callback_context = callbackContext;
        }

        void startCallback() override {
            requestAudio();



            // todo: instruct to create a file and prepare to write data to it
        }

        void stopCallback() override {
            // todo: instruct to flush buffer (if i implement a buffer, as i should)
            // and to save the file
            // flush file
        }

        // todo: write to file descriptor passed from kotlin?
        // https://developer.android.com/ndk/reference/group/file-descriptor
        void write(aap::AudioBuffer *audioDataToWrite, int32_t bufferPosition, int32_t numFrames) override {
            // write data to file and buffer (idk why we would need it in the buffer tbh)
            choc::buffer::FrameRange range{0, (uint32_t ) numFrames};
            choc::buffer::copy(aap_buffer.audio.getFrameRange(range), audioDataToWrite->audio.getView().getFrameRange(range));
            if (fileWriter) {
                fileWriter->appendFrames(audioDataToWrite->audio);
            }

            // clear the buffers to prepare for new request
            aap_buffer.audio.clear();
            memset(aap_buffer.midi_in, 0, aap_buffer.midi_capacity);
            memset(aap_buffer.midi_out, 0, aap_buffer.midi_capacity);
            // memset(oboeAudioData, 0, numFrames * sizeof(float));

            // make new request
            requestAudio();
        }

    private:

        // request audio from the framework
        void requestAudio() {
            // kick callback to generate some data
            aap_callback(callback_context, &aap_buffer, framesPerCallback);
        }

    };

} // aap

#endif //AAP_CORE_FILEAUDIODEVICEOUT_H
