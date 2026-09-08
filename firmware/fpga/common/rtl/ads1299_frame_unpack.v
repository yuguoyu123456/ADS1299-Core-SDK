`timescale 1ns/1ps
/* Unpack one ADS1299-8 216-bit frame captured MSB-first. */
module ads1299_frame_unpack (
    input  wire [215:0] frame_data,
    output wire [23:0]  status,
    output wire signed [23:0] ch1,
    output wire signed [23:0] ch2,
    output wire signed [23:0] ch3,
    output wire signed [23:0] ch4,
    output wire signed [23:0] ch5,
    output wire signed [23:0] ch6,
    output wire signed [23:0] ch7,
    output wire signed [23:0] ch8
);
    assign status = frame_data[215:192];
    assign ch1 = frame_data[191:168];
    assign ch2 = frame_data[167:144];
    assign ch3 = frame_data[143:120];
    assign ch4 = frame_data[119:96];
    assign ch5 = frame_data[95:72];
    assign ch6 = frame_data[71:48];
    assign ch7 = frame_data[47:24];
    assign ch8 = frame_data[23:0];
endmodule
