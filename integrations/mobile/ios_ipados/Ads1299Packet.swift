import Foundation

public struct Ads1299Packet: Sendable {
    public let version: UInt8
    public let flags: UInt8
    public let sequence: UInt32
    public let timestampUs: UInt32
    public let status: [UInt8]
    public let channels: [Int32]
    public let crc: UInt16
    public let raw: Data
}

public enum Ads1299ProtocolError: Error, CustomStringConvertible {
    case size(Int)
    case sync
    case version(UInt8)
    case crc(expected: UInt16, actual: UInt16)

    public var description: String {
        switch self {
        case .size(let value): return "packet size \(value) != 49"
        case .sync: return "bad ADS1299 packet sync"
        case .version(let value): return "unsupported protocol version \(value)"
        case .crc(let expected, let actual):
            return String(format: "CRC mismatch wire=0x%04X expected=0x%04X", actual, expected)
        }
    }
}

public enum Ads1299Protocol {
    public static let packetSize = 49
    public static let channels = 8
    public static let version: UInt8 = 0
    public static let sync0: UInt8 = 0xA5
    public static let sync1: UInt8 = 0x5A
    private static let crcOffset = 47

    public static func crc16CcittFalse(_ bytes: Data) -> UInt16 {
        var crc: UInt16 = 0xFFFF
        for byte in bytes {
            crc ^= UInt16(byte) << 8
            for _ in 0..<8 {
                if (crc & 0x8000) != 0 {
                    crc = (crc << 1) ^ 0x1021
                } else {
                    crc <<= 1
                }
            }
        }
        return crc
    }

    public static func decode(_ raw: Data) throws -> Ads1299Packet {
        guard raw.count == packetSize else { throw Ads1299ProtocolError.size(raw.count) }
        guard raw[0] == sync0 && raw[1] == sync1 else { throw Ads1299ProtocolError.sync }
        guard raw[2] == version else { throw Ads1299ProtocolError.version(raw[2]) }

        let expected = crc16CcittFalse(raw.subdata(in: 2..<crcOffset))
        let actual = u16le(raw, crcOffset)
        guard actual == expected else {
            throw Ads1299ProtocolError.crc(expected: expected, actual: actual)
        }

        let status = Array(raw[12..<15])
        var channelValues: [Int32] = []
        channelValues.reserveCapacity(channels)
        for channel in 0..<channels {
            channelValues.append(i32le(raw, 15 + channel * 4))
        }

        return Ads1299Packet(
            version: raw[2],
            flags: raw[3],
            sequence: u32le(raw, 4),
            timestampUs: u32le(raw, 8),
            status: status,
            channels: channelValues,
            crc: actual,
            raw: raw
        )
    }

    public static func codeToMicrovolts(_ code: Int32, vrefVolts: Double, gain: UInt) -> Double {
        guard vrefVolts > 0, gain > 0 else { return 0 }
        return Double(code) * vrefVolts / (Double(gain) * 8_388_607.0) * 1_000_000.0
    }

    private static func u16le(_ data: Data, _ offset: Int) -> UInt16 {
        UInt16(data[offset]) | (UInt16(data[offset + 1]) << 8)
    }

    private static func u32le(_ data: Data, _ offset: Int) -> UInt32 {
        UInt32(data[offset]) |
            (UInt32(data[offset + 1]) << 8) |
            (UInt32(data[offset + 2]) << 16) |
            (UInt32(data[offset + 3]) << 24)
    }

    private static func i32le(_ data: Data, _ offset: Int) -> Int32 {
        Int32(bitPattern: u32le(data, offset))
    }
}
