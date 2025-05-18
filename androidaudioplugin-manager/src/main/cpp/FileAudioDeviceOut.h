//
// Created by leoni on 18.05.2025.
//

#ifndef AAP_CORE_FILEAUDIODEVICEOUT_H
#define AAP_CORE_FILEAUDIODEVICEOUT_H

#include "AudioDevice.h"

namespace aap {

    class FileAudioDeviceOut : public AudioDeviceOut {
        // todo: write to file descriptor passed from kotlin?
        // https://developer.android.com/ndk/reference/group/file-descriptor
    };

} // aap

#endif //AAP_CORE_FILEAUDIODEVICEOUT_H
