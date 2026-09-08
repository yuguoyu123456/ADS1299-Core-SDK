`timescale 1ns/1ps
/*
 * Portable ADS1299 continuous-data capture core.
 *
 * Assumptions:
 *   - ADS1299 is already configured and placed in RDATAC mode.
 *   - SPI mode 1 timing is used (SCLK idle low, DOUT sampled on falling edge).
 *   - One ADS1299-8 conversion frame is 27 bytes = 216 bits.
 *   - CS is controlled by this module and remains low for the whole frame.
 *
 * The module is vendor-neutral and synthesizable on AMD/Xilinx, Intel,
 * Gowin, Lattice, Efinix, Anlogic, Tang and Microchip FPGA toolchains.
 */
module ads1299_capture_core #(
    parameter integer CLOCK_DIV = 4,
    parameter integer FRAME_BITS = 216
) (
    input  wire                  clk,
    input  wire                  rst_n,
    input  wire                  enable,

    input  wire                  ads_drdy_n,
    input  wire                  ads_miso,
    output reg                   ads_cs_n,
    output reg                   ads_sclk,
    output wire                  ads_mosi,

    output reg                   busy,
    output reg                   frame_valid,
    output reg  [FRAME_BITS-1:0] frame_data,
    output reg                   missed_drdy
);
    localparam integer COUNT_W = (FRAME_BITS <= 2) ? 1 : $clog2(FRAME_BITS + 1);
    localparam integer DIV_W   = (CLOCK_DIV <= 2) ? 1 : $clog2(CLOCK_DIV);

    reg [1:0] drdy_sync;
    reg [DIV_W-1:0] div_count;
    reg [COUNT_W-1:0] bit_count;
    reg [FRAME_BITS-1:0] shift_reg;

    assign ads_mosi = 1'b0; /* RDATAC read requires no command bytes. */

    wire drdy_falling = (drdy_sync == 2'b10);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            drdy_sync <= 2'b11;
        end else begin
            drdy_sync <= {drdy_sync[0], ads_drdy_n};
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            ads_cs_n    <= 1'b1;
            ads_sclk    <= 1'b0;
            busy        <= 1'b0;
            frame_valid <= 1'b0;
            missed_drdy <= 1'b0;
            div_count   <= {DIV_W{1'b0}};
            bit_count   <= {COUNT_W{1'b0}};
            shift_reg   <= {FRAME_BITS{1'b0}};
            frame_data  <= {FRAME_BITS{1'b0}};
        end else begin
            frame_valid <= 1'b0;

            if (!enable) begin
                ads_cs_n  <= 1'b1;
                ads_sclk  <= 1'b0;
                busy      <= 1'b0;
                div_count <= {DIV_W{1'b0}};
                bit_count <= {COUNT_W{1'b0}};
            end else begin
                if (drdy_falling && busy)
                    missed_drdy <= 1'b1;

                if (drdy_falling && !busy) begin
                    ads_cs_n  <= 1'b0;
                    ads_sclk  <= 1'b0;
                    busy      <= 1'b1;
                    div_count <= {DIV_W{1'b0}};
                    bit_count <= {COUNT_W{1'b0}};
                    shift_reg <= {FRAME_BITS{1'b0}};
                end else if (busy) begin
                    if (div_count == CLOCK_DIV - 1) begin
                        div_count <= {DIV_W{1'b0}};

                        if (!ads_sclk) begin
                            /* Rising edge: ADS1299 advances DOUT for mode 1. */
                            ads_sclk <= 1'b1;
                        end else begin
                            /* Falling edge: sample one DOUT bit. */
                            ads_sclk  <= 1'b0;
                            shift_reg <= {shift_reg[FRAME_BITS-2:0], ads_miso};

                            if (bit_count == FRAME_BITS - 1) begin
                                frame_data  <= {shift_reg[FRAME_BITS-2:0], ads_miso};
                                frame_valid <= 1'b1;
                                busy        <= 1'b0;
                                ads_cs_n    <= 1'b1;
                                bit_count   <= {COUNT_W{1'b0}};
                            end else begin
                                bit_count <= bit_count + 1'b1;
                            end
                        end
                    end else begin
                        div_count <= div_count + 1'b1;
                    end
                end
            end
        end
    end
endmodule
