`timescale 1ns/1ps
// Byte-oriented SPI Mode-1 master. ADS1299 protocol semantics stay upstream.
module ads1299_spi_master #(
    parameter integer CLOCK_DIV = 4
) (
    input  wire       clk,
    input  wire       rst_n,
    input  wire       start,
    input  wire [7:0] tx_data,
    output reg  [7:0] rx_data,
    output reg        busy,
    output reg        done,
    output reg        sclk,
    output reg        mosi,
    input  wire       miso
);
    integer divider_count;
    reg [2:0] bit_index;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rx_data <= 8'h00; busy <= 1'b0; done <= 1'b0;
            sclk <= 1'b0; mosi <= 1'b0; divider_count <= 0; bit_index <= 3'd7;
        end else begin
            done <= 1'b0;
            if (start && !busy) begin
                busy <= 1'b1; sclk <= 1'b0; bit_index <= 3'd7;
                divider_count <= 0; mosi <= tx_data[7]; rx_data <= 8'h00;
            end else if (busy) begin
                if (divider_count == CLOCK_DIV - 1) begin
                    divider_count <= 0;
                    if (!sclk) begin
                        // Mode 1: data changes after the rising edge.
                        sclk <= 1'b1;
                    end else begin
                        // Mode 1: sample immediately before the falling edge.
                        sclk <= 1'b0;
                        rx_data[bit_index] <= miso;
                        if (bit_index == 0) begin
                            busy <= 1'b0; done <= 1'b1; mosi <= 1'b0;
                        end else begin
                            bit_index <= bit_index - 1'b1;
                            mosi <= tx_data[bit_index - 1'b1];
                        end
                    end
                end else begin
                    divider_count <= divider_count + 1;
                end
            end
        end
    end
endmodule
