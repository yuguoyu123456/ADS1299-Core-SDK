`timescale 1ns/1ps
// Variant-aware ADS1299-x frame parser. Register policy stays in the Core.
module frame_parser #(
    parameter integer CHANNEL_COUNT = 8
) (
    input  wire         clk,
    input  wire         rst_n,
    input  wire         frame_start,
    input  wire         byte_valid,
    input  wire [7:0]   byte_data,
    output reg  [23:0]  status,
    output reg  [191:0] channels,
    output reg          frame_valid,
    output reg          frame_error
);
    reg [5:0] byte_index;
    reg       active;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            status <= 24'h0; channels <= 192'h0; frame_valid <= 1'b0;
            frame_error <= 1'b0; byte_index <= 0; active <= 1'b0;
        end else begin
            frame_valid <= 1'b0; frame_error <= 1'b0;
            if (frame_start) begin
                if (active) frame_error <= 1'b1;
                active <= 1'b1; byte_index <= 0;
                status <= 24'h0; channels <= 192'h0;
            end
            if (byte_valid) begin
                if (!active) begin
                    frame_error <= 1'b1;
                end else begin
                    if (byte_index == 0 && byte_data[7:4] != 4'hC)
                        frame_error <= 1'b1;
                    if (byte_index < 3)
                        status <= {status[15:0], byte_data};
                    else
                        channels[191 - ((byte_index - 3) * 8) -: 8] <= byte_data;
                    if (byte_index == (2 + CHANNEL_COUNT * 3)) begin
                        frame_valid <= 1'b1; active <= 1'b0; byte_index <= 0;
                    end else begin
                        byte_index <= byte_index + 1'b1;
                    end
                end
            end
        end
    end

    initial begin
        if (CHANNEL_COUNT != 4 && CHANNEL_COUNT != 6 && CHANNEL_COUNT != 8)
            $error("CHANNEL_COUNT must be 4, 6 or 8");
    end
endmodule
