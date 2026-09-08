import Foundation

public final class Ads1299StreamParser: @unchecked Sendable {
    private var buffer = Data()

    public private(set) var validPackets: UInt64 = 0
    public private(set) var formatErrors: UInt64 = 0
    public private(set) var discardedBytes: UInt64 = 0

    public init() {}

    public func reset() {
        buffer.removeAll(keepingCapacity: true)
        validPackets = 0
        formatErrors = 0
        discardedBytes = 0
    }

    public func feed(_ chunk: Data) -> [Ads1299Packet] {
        if !chunk.isEmpty { buffer.append(chunk) }
        var output: [Ads1299Packet] = []

        while true {
            guard let syncIndex = findSync() else {
                if buffer.last == Ads1299Protocol.sync0 {
                    if buffer.count > 1 {
                        discardedBytes += UInt64(buffer.count - 1)
                        buffer = Data([Ads1299Protocol.sync0])
                    }
                } else {
                    discardedBytes += UInt64(buffer.count)
                    buffer.removeAll(keepingCapacity: true)
                }
                break
            }

            if syncIndex > 0 {
                discardedBytes += UInt64(syncIndex)
                buffer.removeSubrange(0..<syncIndex)
            }

            if buffer.count < Ads1299Protocol.packetSize { break }
            let candidate = buffer.subdata(in: 0..<Ads1299Protocol.packetSize)

            do {
                let packet = try Ads1299Protocol.decode(candidate)
                output.append(packet)
                validPackets += 1
                buffer.removeSubrange(0..<Ads1299Protocol.packetSize)
            } catch {
                formatErrors += 1
                discardedBytes += 1
                buffer.removeFirst()
            }
        }

        return output
    }

    private func findSync() -> Int? {
        guard buffer.count >= 2 else { return nil }
        for index in 0..<(buffer.count - 1) {
            if buffer[index] == Ads1299Protocol.sync0 &&
                buffer[index + 1] == Ads1299Protocol.sync1 {
                return index
            }
        }
        return nil
    }
}

public final class Ads1299SequenceTracker: @unchecked Sendable {
    public private(set) var last: UInt32?
    public private(set) var gapEvents: UInt64 = 0
    public private(set) var missingPackets: UInt64 = 0
    public private(set) var duplicatesOrReordered: UInt64 = 0

    public init() {}

    public func reset() {
        last = nil
        gapEvents = 0
        missingPackets = 0
        duplicatesOrReordered = 0
    }

    public func observe(_ sequence: UInt32) {
        guard let previous = last else {
            last = sequence
            return
        }

        let expected = previous &+ 1
        if sequence == expected {
            last = sequence
            return
        }
        if sequence == previous {
            duplicatesOrReordered += 1
            return
        }

        let forward = sequence &- expected
        if forward < 0x8000_0000 {
            gapEvents += 1
            missingPackets += UInt64(forward)
            last = sequence
        } else {
            duplicatesOrReordered += 1
        }
    }
}
