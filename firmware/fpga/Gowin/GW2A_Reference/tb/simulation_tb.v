`timescale 1ns/1ps
module simulation_tb;
    reg clk = 0, rst_n = 0, frame_start = 0, byte_valid = 0;
    reg [7:0] byte_data = 0;
    wire [23:0] status; wire [191:0] channels;
    wire frame_valid, frame_error;
    reg ctrl_request = 0, ctrl_tx_valid = 0;
    reg [7:0] ctrl_count = 0, ctrl_tx_byte = 0;
    wire ctrl_tx_ready, ctrl_rx_valid, ctrl_cs_n, ctrl_spi_start, ctrl_busy;
    wire [7:0] ctrl_rx_byte, ctrl_spi_tx;
    wire [7:0] spi_rx;
    wire spi_busy, spi_done, spi_sclk, spi_mosi;
    integer i;

    frame_parser #(.CHANNEL_COUNT(8)) parser(
        .clk(clk), .rst_n(rst_n), .frame_start(frame_start),
        .byte_valid(byte_valid), .byte_data(byte_data), .status(status),
        .channels(channels), .frame_valid(frame_valid), .frame_error(frame_error)
    );
    ads1299_controller controller(
        .clk(clk), .rst_n(rst_n), .request(ctrl_request),
        .byte_count(ctrl_count), .tx_byte(ctrl_tx_byte),
        .tx_valid(ctrl_tx_valid), .tx_ready(ctrl_tx_ready),
        .rx_byte(ctrl_rx_byte), .rx_valid(ctrl_rx_valid), .cs_n(ctrl_cs_n),
        .spi_start(ctrl_spi_start), .spi_tx(ctrl_spi_tx),
        .spi_rx(spi_rx), .spi_busy(spi_busy), .spi_done(spi_done),
        .busy(ctrl_busy)
    );
    ads1299_spi_master #(.CLOCK_DIV(2)) spi(
        .clk(clk), .rst_n(rst_n), .start(ctrl_spi_start), .tx_data(ctrl_spi_tx),
        .rx_data(spi_rx), .busy(spi_busy), .done(spi_done),
        .sclk(spi_sclk), .mosi(spi_mosi), .miso(spi_mosi)
    );
    always #5 clk = ~clk;

    task send_byte(input [7:0] value);
        begin
            @(negedge clk); byte_data = value; byte_valid = 1;
            @(negedge clk); byte_valid = 0;
        end
    endtask

    initial begin
        #20; rst_n = 1;
        @(negedge clk); ctrl_count = 1; ctrl_request = 1;
        @(negedge clk); ctrl_request = 0;
        wait(ctrl_tx_ready);
        @(negedge clk); ctrl_tx_byte = 8'hA5; ctrl_tx_valid = 1;
        @(negedge clk); ctrl_tx_valid = 0;
        wait(ctrl_rx_valid);
        if (ctrl_rx_byte !== 8'hA5) $fatal(1, "controller/SPI loopback mismatch");
        if (!ctrl_cs_n || ctrl_busy) $fatal(1, "controller transaction did not close");
        @(negedge clk); frame_start = 1;
        @(negedge clk); frame_start = 0;
        send_byte(8'hC0); send_byte(8'h00); send_byte(8'h00);
        for (i = 1; i <= 24; i = i + 1) send_byte(i[7:0]);
        if (!frame_valid) $fatal(1, "frame_valid missing");
        if (status !== 24'hC00000) $fatal(1, "status mismatch");
        if (channels[191:184] !== 8'h01 || channels[7:0] !== 8'h18)
            $fatal(1, "channel byte order mismatch");
        if (frame_error) $fatal(1, "unexpected frame error");
        $display("ADS1299 FPGA parser test passed");
        $finish;
    end
endmodule
