`timescale 1ns/1ps

module ads1299_capture_core_tb;
    localparam integer CLOCK_DIV = 2;
    localparam integer FRAME_BITS = 216;

    reg clk = 1'b0;
    reg rst_n = 1'b0;
    reg enable = 1'b0;
    reg ads_drdy_n = 1'b1;
    reg ads_miso = 1'b0;

    wire ads_cs_n;
    wire ads_sclk;
    wire ads_mosi;
    wire busy;
    wire frame_valid;
    wire [215:0] frame_data;
    wire missed_drdy;

    reg [215:0] expected;
    integer tx_bit;

    always #5 clk = ~clk;

    ads1299_capture_core #(
        .CLOCK_DIV(CLOCK_DIV),
        .FRAME_BITS(FRAME_BITS)
    ) dut (
        .clk(clk),
        .rst_n(rst_n),
        .enable(enable),
        .ads_drdy_n(ads_drdy_n),
        .ads_miso(ads_miso),
        .ads_cs_n(ads_cs_n),
        .ads_sclk(ads_sclk),
        .ads_mosi(ads_mosi),
        .busy(busy),
        .frame_valid(frame_valid),
        .frame_data(frame_data),
        .missed_drdy(missed_drdy)
    );

    /* Drive next ADS1299 DOUT bit while SCLK is high so DUT samples it
     * on the following falling edge. */
    always @(posedge ads_sclk) begin
        if (!ads_cs_n && tx_bit >= 0) begin
            ads_miso <= expected[tx_bit];
            tx_bit <= tx_bit - 1;
        end
    end

    initial begin
        expected = {
            24'hC01234,
            24'h000001,
            24'h7FFFFF,
            24'h800000,
            24'hFFFFFF,
            24'h001234,
            24'hFEDCBA,
            24'h010203,
            24'hA0B0C0
        };
        tx_bit = 215;

        #100;
        rst_n = 1'b1;
        enable = 1'b1;
        #100;

        /* Generate a DRDY falling edge. */
        ads_drdy_n = 1'b0;
        #30;
        ads_drdy_n = 1'b1;

        wait(frame_valid === 1'b1);
        #1;
        if (frame_data !== expected) begin
            $display("FAIL: frame mismatch\nexpected=%h\nactual  =%h", expected, frame_data);
            $fatal(1);
        end
        if (missed_drdy !== 1'b0) begin
            $display("FAIL: missed_drdy unexpectedly set");
            $fatal(1);
        end

        $display("PASS: ADS1299 capture core captured 216-bit frame correctly");
        #50;
        $finish;
    end

    initial begin
        #200000;
        $display("FAIL: timeout");
        $fatal(1);
    end
endmodule
