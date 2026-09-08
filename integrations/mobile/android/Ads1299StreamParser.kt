package ads1299.core

class Ads1299StreamParser {
    private var buffer = ByteArray(0)

    var validPackets: Long = 0
        private set
    var formatErrors: Long = 0
        private set
    var discardedBytes: Long = 0
        private set

    fun reset() {
        buffer = ByteArray(0)
        validPackets = 0
        formatErrors = 0
        discardedBytes = 0
    }

    fun feed(chunk: ByteArray): List<Ads1299Packet> {
        if (chunk.isNotEmpty()) {
            val merged = ByteArray(buffer.size + chunk.size)
            buffer.copyInto(merged, 0)
            chunk.copyInto(merged, buffer.size)
            buffer = merged
        }

        val out = mutableListOf<Ads1299Packet>()
        while (true) {
            val sync = findSync(buffer)
            if (sync < 0) {
                if (buffer.isNotEmpty() && (buffer.last().toInt() and 0xFF) == Ads1299Protocol.SYNC0) {
                    discardedBytes += (buffer.size - 1).toLong()
                    buffer = byteArrayOf(buffer.last())
                } else {
                    discardedBytes += buffer.size.toLong()
                    buffer = ByteArray(0)
                }
                break
            }

            if (sync > 0) {
                discardedBytes += sync.toLong()
                buffer = buffer.copyOfRange(sync, buffer.size)
            }

            if (buffer.size < Ads1299Protocol.PACKET_SIZE) break

            val candidate = buffer.copyOfRange(0, Ads1299Protocol.PACKET_SIZE)
            val decoded = try {
                Ads1299Protocol.decode(candidate)
            } catch (_: IllegalArgumentException) {
                null
            }

            if (decoded == null) {
                formatErrors += 1
                discardedBytes += 1
                buffer = buffer.copyOfRange(1, buffer.size)
                continue
            }

            out += decoded
            validPackets += 1
            buffer = buffer.copyOfRange(Ads1299Protocol.PACKET_SIZE, buffer.size)
        }
        return out
    }

    private fun findSync(data: ByteArray): Int {
        if (data.size < 2) return -1
        for (i in 0 until data.size - 1) {
            if ((data[i].toInt() and 0xFF) == Ads1299Protocol.SYNC0 &&
                (data[i + 1].toInt() and 0xFF) == Ads1299Protocol.SYNC1
            ) return i
        }
        return -1
    }
}

class Ads1299SequenceTracker {
    var last: Long? = null
        private set
    var gapEvents: Long = 0
        private set
    var missingPackets: Long = 0
        private set
    var duplicatesOrReordered: Long = 0
        private set

    fun observe(sequence: Long) {
        require(sequence in 0..0xFFFFFFFFL)
        val previous = last
        if (previous == null) {
            last = sequence
            return
        }

        val expected = (previous + 1L) and 0xFFFFFFFFL
        if (sequence == expected) {
            last = sequence
            return
        }
        if (sequence == previous) {
            duplicatesOrReordered += 1
            return
        }

        val forward = (sequence - expected) and 0xFFFFFFFFL
        if (forward < 0x80000000L) {
            gapEvents += 1
            missingPackets += forward
            last = sequence
        } else {
            duplicatesOrReordered += 1
        }
    }
}
