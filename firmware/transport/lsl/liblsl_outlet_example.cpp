#include <array>
#include <cstdint>
#include <iostream>
#include <string>

#include <lsl_cpp.h>

extern "C" {
#include "ads1299_lsl_bridge.h"
}

/*
 * Minimal host-side example.
 *
 * Integrate this helper after your USB/UART/Ethernet/Wi-Fi receiver has
 * reconstructed exactly one canonical ADS1299 packet.
 *
 * Build/link against liblsl and the common ADS1299 packet + LSL bridge files.
 */
class Ads1299LslOutlet {
public:
    Ads1299LslOutlet(const std::string &stream_name,
                     const std::string &source_id,
                     double sample_rate_hz,
                     double vref_volts,
                     unsigned gain)
        : info_(stream_name,
                "EEG",
                ADS1299_PACKET_CHANNELS,
                sample_rate_hz,
                lsl::cf_float32,
                source_id),
          outlet_(configure_metadata(info_, sample_rate_hz, vref_volts, gain)),
          vref_volts_(vref_volts),
          gain_(gain)
    {
    }

    bool push_packet(const std::uint8_t packet[ADS1299_PACKET_SIZE])
    {
        ads1299_lsl_sample_t decoded{};
        std::array<float, ADS1299_PACKET_CHANNELS> channels{};

        if (ads1299_lsl_packet_to_sample(packet,
                                         ADS1299_PACKET_SIZE,
                                         vref_volts_,
                                         gain_,
                                         &decoded) != 0) {
            return false;
        }

        for (std::size_t i = 0; i < channels.size(); ++i)
            channels[i] = decoded.channels_uv[i];

        /* Baseline timestamp policy: stamp at host receipt using LSL's clock.
         * Do not pass raw device_timestamp_us as an LSL absolute timestamp.
         * A production bridge can estimate a device-clock -> lsl::local_clock()
         * mapping and then push calibrated timestamps instead.
         */
        outlet_.push_sample(channels.data(), lsl::local_clock());
        return true;
    }

private:
    static lsl::stream_info configure_metadata(lsl::stream_info info,
                                               double sample_rate_hz,
                                               double vref_volts,
                                               unsigned gain)
    {
        auto desc = info.desc();
        desc.append_child_value("manufacturer", "ADS1299-Core-SDK");
        desc.append_child_value("sample_rate_hz", std::to_string(sample_rate_hz).c_str());
        desc.append_child_value("vref_volts", std::to_string(vref_volts).c_str());
        desc.append_child_value("pga_gain", std::to_string(gain).c_str());
        desc.append_child_value("unit", "microvolts");

        auto channels = desc.append_child("channels");
        for (unsigned i = 0; i < ADS1299_PACKET_CHANNELS; ++i) {
            auto ch = channels.append_child("channel");
            std::string label = "EEG" + std::to_string(i + 1u);
            ch.append_child_value("label", label.c_str());
            ch.append_child_value("unit", "microvolts");
            ch.append_child_value("type", "EEG");
        }
        return info;
    }

    lsl::stream_info info_;
    lsl::stream_outlet outlet_;
    double vref_volts_;
    unsigned gain_;
};

int main()
{
    std::cout << "ADS1299 liblsl outlet example compiled successfully.\n"
              << "Feed canonical 49-byte packets into Ads1299LslOutlet::push_packet() "
              << "from your selected transport receiver.\n";
    return 0;
}
