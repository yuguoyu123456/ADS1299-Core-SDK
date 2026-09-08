`timescale 1ns/1ps
/*
 * Convert a captured 216-bit ADS1299 frame into a 27-byte ready/valid stream.
 * Byte 0 is STATUS[23:16], byte 1 STATUS[15:8], byte 2 STATUS[7:0], then CH1..CH8.
 */
module ads1299_frame_to_bytes (
    input  wire         clk,
    input  wire         rst_n,
    input  wire [215:0] frame_data,
    input  wire         frame_valid,

    output reg  [7:0]   byte_data,
    output reg          byte_valid,
    input  wire         byte_ready,
    output reg          frame_busy,
    output reg          frame_done
);
    reg [215:0] shift_reg;
    reg [5:0] byte_index;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            shift_reg <= 216'd0;
            byte_index <= 6'd0;
            byte_data <= 8'd0;
            byte_valid <= 1'b0;
            frame_busy <= 1'b0;
            frame_done <= 1'b0;
        end else begin
            frame_done <= 1'b0;

            if (frame_valid && !frame_busy) begin
                shift_reg <= frame_data;
                byte_index <= 6'd0;
                byte_data <= frame_data[215:208];
                byte_valid <= 1'b1;
                frame_busy <= 1'b1;
            end else if (frame_busy && byte_valid && byte_ready) begin
                if (byte_index == 6'd26) begin
                    byte_valid <= 1'b0;
                    frame_busy <= 1'b0;
                    frame_done <= 1'b1;
                end else begin
                    shift_reg <= {shift_reg[207:0], 8'h00};
                    byte_index <= byte_index + 1'b1;
                    byte_data <= shift_reg[207:200];
                    byte_valid <= 1'b1;
                end
            end
        end
    end
endmodule
