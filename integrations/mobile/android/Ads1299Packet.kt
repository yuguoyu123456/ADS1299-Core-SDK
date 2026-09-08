package ads1299.core

data class Ads1299Packet(
    val version: Int,
    val flags: Int,
    val sequence: Long,
    val timestampUs: Long,
    val status: ByteArray,
    val channels: IntArray,
    val crc: Int,
    val raw: ByteArray,
)

object Ads1299Protocol {
    const val PACKET_SIZE = 49
    const val CHANNELS = 8
    const val VERSION = 0
    const val SYNC0 = 0xA5
    const val SYNC1 = 0x5A
    private const val CRC_OFFSET = 47

    fun crc16CcittFalse(data: ByteArray, offset: Int = 0, length: Int = data.size): Int {
        require(offset >= 0 && length >= 0 && offset + length <= data.size)
        var crc = 0xFFFF
        for (i in offset until offset + length) {
            crc = crc xor ((data[i].toInt() and 0xFF) shl 8)
            repeat(8) {
                crc = if ((crc and 0x8000) != 0) {
                    ((crc shl 1) xor 0x1021) and 0xFFFF
                } else {
                    (crc shl 1) and 0xFFFF
                }
            }
        }
        return crc
    }

    fun decode(raw: ByteArray): Ads1299Packet {
        require(raw.size == PACKET_SIZE) { "packet size ${raw.size} != $PACKET_SIZE" }
        require(u8(raw[0]) == SYNC0 && u8(raw[1]) == SYNC1) { "bad sync" }
        require(u8(raw[2]) == VERSION) { "unsupported version ${u8(raw[2])}" }

        val expected = crc16CcittFalse(raw, 2, CRC_OFFSET - 2)
        val actual = u16le(raw, CRC_OFFSET)
        require(actual == expected) {
            "CRC mismatch wire=0x${actual.toString(16)} expected=0x${expected.toString(16)}"
        }

        val status = raw.copyOfRange(12, 15)
        val channels = IntArray(CHANNELS) { ch -> i32le(raw, 15 + ch * 4) }

        return Ads1299Packet(
            version = u8(raw[2]),
            flags = u8(raw[3]),
            sequence = u32le(raw, 4),
            timestampUs = u32le(raw, 8),
            status = status,
            channels = channels,
            crc = actual,
            raw = raw.copyOf(),
        )
    }

    fun codeToMicrovolts(code: Int, vrefVolts: Double, gain: Int): Double {
        require(vrefVolts > 0.0)
        require(gain > 0)
        return code.toDouble() * vrefVolts / (gain.toDouble() * 8388607.0) * 1_000_000.0
    }

    private fun u8(value: Byte): Int = value.toInt() and 0xFF

    private fun u16le(data: ByteArray, offset: Int): Int =
        u8(data[offset]) or (u8(data[offset + 1]) shl 8)

    private fun u32le(data: ByteArray, offset: Int): Long =
        (u8(data[offset]).toLong()) or
            (u8(data[offset + 1]).toLong() shl 8) or
            (u8(data[offset + 2]).toLong() shl 16) or
            (u8(data[offset + 3]).toLong() shl 24)

    private fun i32le(data: ByteArray, offset: Int): Int =
        u8(data[offset]) or
            (u8(data[offset + 1]) shl 8) or
            (u8(data[offset + 2]) shl 16) or
            (u8(data[offset + 3]) shl 24)
}
