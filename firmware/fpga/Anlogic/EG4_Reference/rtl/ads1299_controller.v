`timescale 1ns/1ps
// Transaction controller. It transports opaque bytes and owns no ADS1299
// register addresses, bit fields, channel configuration or test profiles.
module ads1299_controller (
    input  wire       clk,
    input  wire       rst_n,
    input  wire       request,
    input  wire [7:0] byte_count,
    input  wire [7:0] tx_byte,
    input  wire       tx_valid,
    output reg        tx_ready,
    output reg  [7:0] rx_byte,
    output reg        rx_valid,
    output reg        cs_n,
    output reg        spi_start,
    output reg  [7:0] spi_tx,
    input  wire [7:0] spi_rx,
    input  wire       spi_busy,
    input  wire       spi_done,
    output reg        busy
);
    reg [7:0] remaining;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            tx_ready <= 1'b0; rx_byte <= 8'h00; rx_valid <= 1'b0;
            cs_n <= 1'b1; spi_start <= 1'b0; spi_tx <= 8'h00;
            busy <= 1'b0; remaining <= 8'h00;
        end else begin
            spi_start <= 1'b0; rx_valid <= 1'b0;
            if (request && !busy && byte_count != 0) begin
                busy <= 1'b1; cs_n <= 1'b0; remaining <= byte_count;
                tx_ready <= 1'b1;
            end else if (busy) begin
                if (tx_ready && tx_valid && !spi_busy) begin
                    spi_tx <= tx_byte; spi_start <= 1'b1; tx_ready <= 1'b0;
                end
                if (spi_done) begin
                    rx_byte <= spi_rx; rx_valid <= 1'b1;
                    if (remaining == 1) begin
                        remaining <= 0; busy <= 1'b0; cs_n <= 1'b1;
                    end else begin
                        remaining <= remaining - 1'b1; tx_ready <= 1'b1;
                    end
                end
            end
        end
    end
endmodule
