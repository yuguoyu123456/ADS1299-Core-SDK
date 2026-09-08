package ads1299.core

private fun putU32le(out: ByteArray, offset: Int, value: Long) {
    out[offset] = (value and 0xFF).toByte()
    out[offset + 1] = ((value shr 8) and 0xFF).toByte()
    out[offset + 2] = ((value shr 16) and 0xFF).toByte()
    out[offset + 3] = ((value shr 24) and 0xFF).toByte()
}

private fun putI32le(out: ByteArray, offset: Int, value: Int) =
    putU32le(out, offset, value.toLong() and 0xFFFFFFFFL)

private fun makePacket(sequence: Long, timestampUs: Long): ByteArray {
    val out = ByteArray(Ads1299Protocol.PACKET_SIZE)
    out[0] = 0xA5.toByte()
    out[1] = 0x5A.toByte()
    out[2] = 0
    out[3] = 0x21
    putU32le(out, 4, sequence)
    putU32le(out, 8, timestampUs)
    out[12] = 0xC0.toByte()
    out[13] = 0
    out[14] = 0
    val values = intArrayOf(-8388608, -123456, -1, 0, 1, 123456, 4194304, 8388607)
    values.forEachIndexed { index, value -> putI32le(out, 15 + index * 4, value) }
    val crc = Ads1299Protocol.crc16CcittFalse(out, 2, 45)
    out[47] = (crc and 0xFF).toByte()
    out[48] = ((crc shr 8) and 0xFF).toByte()
    return out
}

fun main() {
    val p0 = makePacket(100, 1000)
    val decoded = Ads1299Protocol.decode(p0)
    check(decoded.sequence == 100L)
    check(decoded.timestampUs == 1000L)
    check(decoded.channels.first() == -8388608)
    check(decoded.channels.last() == 8388607)

    val parser = Ads1299StreamParser()
    val p1 = makePacket(101, 2000)
    val stream = byteArrayOf(1, 2, 3, 4) + p0 + p1
    val received = mutableListOf<Ads1299Packet>()
    var offset = 0
    val chunks = intArrayOf(1, 2, 7, 13, 29, 5, 64, 128)
    for (chunkSize in chunks) {
        val end = minOf(stream.size, offset + chunkSize)
        if (end > offset) received += parser.feed(stream.copyOfRange(offset, end))
        offset = end
    }
    if (offset < stream.size) received += parser.feed(stream.copyOfRange(offset, stream.size))
    check(received.map { it.sequence } == listOf(100L, 101L))
    check(parser.discardedBytes >= 4)

    val corrupted = makePacket(102, 3000)
    corrupted[20] = (corrupted[20].toInt() xor 1).toByte()
    val afterCorruption = parser.feed(corrupted + makePacket(103, 4000))
    check(afterCorruption.any { it.sequence == 103L })
    check(parser.formatErrors >= 1)

    val tracker = Ads1299SequenceTracker()
    tracker.observe(10)
    tracker.observe(11)
    tracker.observe(14)
    check(tracker.gapEvents == 1L)
    check(tracker.missingPackets == 2L)

    val wrap = Ads1299SequenceTracker()
    wrap.observe(0xFFFFFFFEL)
    wrap.observe(0xFFFFFFFFL)
    wrap.observe(0)
    check(wrap.missingPackets == 0L)

    println("Android/Kotlin ADS1299 protocol smoke test: PASS")
}
