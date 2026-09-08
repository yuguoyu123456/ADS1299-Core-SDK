import Foundation

private func putU32le(_ data: inout Data, _ offset: Int, _ value: UInt32) {
    data[offset] = UInt8(value & 0xFF)
    data[offset + 1] = UInt8((value >> 8) & 0xFF)
    data[offset + 2] = UInt8((value >> 16) & 0xFF)
    data[offset + 3] = UInt8((value >> 24) & 0xFF)
}

private func makePacket(sequence: UInt32, timestampUs: UInt32) -> Data {
    var out = Data(repeating: 0, count: Ads1299Protocol.packetSize)
    out[0] = 0xA5
    out[1] = 0x5A
    out[2] = 0
    out[3] = 0x21
    putU32le(&out, 4, sequence)
    putU32le(&out, 8, timestampUs)
    out[12] = 0xC0
    out[13] = 0
    out[14] = 0

    let values: [Int32] = [-8_388_608, -123_456, -1, 0, 1, 123_456, 4_194_304, 8_388_607]
    for (index, value) in values.enumerated() {
        putU32le(&out, 15 + index * 4, UInt32(bitPattern: value))
    }

    let crc = Ads1299Protocol.crc16CcittFalse(out.subdata(in: 2..<47))
    out[47] = UInt8(crc & 0xFF)
    out[48] = UInt8((crc >> 8) & 0xFF)
    return out
}

@main
struct Ads1299ProtocolSmokeTest {
    static func main() throws {
        let p0 = makePacket(sequence: 100, timestampUs: 1000)
        let decoded = try Ads1299Protocol.decode(p0)
        precondition(decoded.sequence == 100)
        precondition(decoded.timestampUs == 1000)
        precondition(decoded.channels.first == -8_388_608)
        precondition(decoded.channels.last == 8_388_607)

        let parser = Ads1299StreamParser()
        let p1 = makePacket(sequence: 101, timestampUs: 2000)
        let stream = Data([1, 2, 3, 4]) + p0 + p1
        let chunkSizes = [1, 2, 7, 13, 29, 5, 64, 128]
        var offset = 0
        var received: [Ads1299Packet] = []
        for chunkSize in chunkSizes {
            let end = min(stream.count, offset + chunkSize)
            if end > offset {
                received += parser.feed(stream.subdata(in: offset..<end))
            }
            offset = end
        }
        if offset < stream.count {
            received += parser.feed(stream.subdata(in: offset..<stream.count))
        }
        precondition(received.map(\.sequence) == [100, 101])
        precondition(parser.discardedBytes >= 4)

        var bad = makePacket(sequence: 102, timestampUs: 3000)
        bad[20] ^= 0x01
        let afterCorruption = parser.feed(bad + makePacket(sequence: 103, timestampUs: 4000))
        precondition(afterCorruption.contains(where: { $0.sequence == 103 }))
        precondition(parser.formatErrors >= 1)

        let tracker = Ads1299SequenceTracker()
        tracker.observe(10)
        tracker.observe(11)
        tracker.observe(14)
        precondition(tracker.gapEvents == 1)
        precondition(tracker.missingPackets == 2)

        let wrap = Ads1299SequenceTracker()
        wrap.observe(0xFFFF_FFFE)
        wrap.observe(0xFFFF_FFFF)
        wrap.observe(0)
        precondition(wrap.missingPackets == 0)

        print("iOS/Swift ADS1299 protocol smoke test: PASS")
    }
}
