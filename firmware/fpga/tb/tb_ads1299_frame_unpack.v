`timescale 1ns/1ps
module tb_ads1299_frame_unpack;
    reg [215:0] frame;
    wire [23:0] status;
    wire signed [23:0] ch1, ch2, ch3, ch4, ch5, ch6, ch7, ch8;

    ads1299_frame_unpack dut(
        .frame(frame), .status(status),
        .ch1(ch1), .ch2(ch2), .ch3(ch3), .ch4(ch4),
        .ch5(ch5), .ch6(ch6), .ch7(ch7), .ch8(ch8)
    );

    initial begin
        frame = {24'hC00000,
                 24'h000001, 24'hFFFFFF, 24'h7FFFFF, 24'h800000,
                 24'h00007B, 24'hFFFE38, 24'h00002A, 24'h123456};
        #1;
        if (status !== 24'hC00000) $fatal(1, "status");
        if (ch1 !== 24'sh000001) $fatal(1, "ch1");
        if (ch2 !== -24'sd1) $fatal(1, "ch2");
        if (ch3 !== 24'sh7FFFFF) $fatal(1, "ch3");
        if (ch4 !== -24'sd8388608) $fatal(1, "ch4");
        if (ch5 !== 24'sd123) $fatal(1, "ch5");
        if (ch6 !== -24'sd456) $fatal(1, "ch6");
        if (ch7 !== 24'sd42) $fatal(1, "ch7");
        if (ch8 !== 24'sh123456) $fatal(1, "ch8");
        $display("ADS1299 frame unpack test passed");
        $finish;
    end
endmodule
