// ADS1299 216-bit frame unpacker.
// frame[215:192] = status, followed by CH1..CH8 as 24-bit two's-complement.
module ads1299_frame_unpack (
    input  wire [215:0] frame,
    output wire [23:0] status,
    output wire signed [23:0] ch1,
    output wire signed [23:0] ch2,
    output wire signed [23:0] ch3,
    output wire signed [23:0] ch4,
    output wire signed [23:0] ch5,
    output wire signed [23:0] ch6,
    output wire signed [23:0] ch7,
    output wire signed [23:0] ch8
);
    assign status = frame[215:192];
    assign ch1 = frame[191:168];
    assign ch2 = frame[167:144];
    assign ch3 = frame[143:120];
    assign ch4 = frame[119:96];
    assign ch5 = frame[95:72];
    assign ch6 = frame[71:48];
    assign ch7 = frame[47:24];
    assign ch8 = frame[23:0];
endmodule
