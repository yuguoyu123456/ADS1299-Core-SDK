#!/usr/bin/env python3
"""Generate vendor-neutral, synthesizable ADS1299 FPGA reference packages."""

from pathlib import Path
import json


ROOT = Path(__file__).resolve().parents[1]
FPGA = ROOT / "firmware" / "fpga"

TARGETS = (
    (71, "AMD_Xilinx/Zynq7000_Reference", "AMD Xilinx", "Zynq-7000", "Vivado", "XDC"),
    (72, "AMD_Xilinx/ZynqUltraScalePlus_Reference", "AMD Xilinx", "Zynq UltraScale+", "Vivado", "XDC"),
    (73, "AMD_Xilinx/Artix7_Reference", "AMD Xilinx", "Artix-7", "Vivado", "XDC"),
    (74, "AMD_Xilinx/Kintex7_Reference", "AMD Xilinx", "Kintex-7", "Vivado", "XDC"),
    (75, "Intel_Altera/CycloneV_Reference", "Intel FPGA", "Cyclone V", "Quartus Prime", "QSF"),
    (76, "Intel_Altera/CycloneIV_Reference", "Intel FPGA", "Cyclone IV", "Quartus Prime", "QSF"),
    (77, "Intel_Altera/MAX10_Reference", "Intel FPGA", "MAX 10", "Quartus Prime", "QSF"),
    (78, "Lattice/ECP5_Reference", "Lattice", "ECP5", "Radiant or open-source ECP5 flow", "LPF"),
    (79, "Lattice/CertusNX_Reference", "Lattice", "Certus-NX", "Radiant", "LPF"),
    (80, "Microchip_FPGA/PolarFire_Reference", "Microchip FPGA", "PolarFire", "Libero SoC", "PDC"),
    (81, "Gowin/GW1N_Reference", "Gowin", "GW1N", "Gowin EDA", "CST"),
    (82, "Gowin/GW2A_Reference", "Gowin", "GW2A", "Gowin EDA", "CST"),
    (83, "Anlogic/EG4_Reference", "Anlogic", "EG4", "Tang Dynasty", "ADC"),
    (84, "Tang_FPGA/GW_Series_Reference", "Tang FPGA", "GW series", "Gowin EDA", "CST"),
    (85, "Efinix/Titanium_Reference", "Efinix", "Titanium", "Efinity", "SDC"),
)

SPI_MASTER = r'''`timescale 1ns/1ps
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
'''

CONTROLLER = r'''`timescale 1ns/1ps
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
'''

FRAME_PARSER = r'''`timescale 1ns/1ps
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
'''

TB = r'''`timescale 1ns/1ps
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
'''


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text.rstrip() + "\n", encoding="utf-8")


def main() -> None:
    for rank, relative, vendor, family, tool, constraint_kind in TARGETS:
        base = FPGA / relative
        write(base / "README.md", f"""# {vendor} {family} ADS1299 FPGA reference\n\nGlobal ecosystem rank: **{rank}**. Status: **Reference**. No board, synthesis, timing or hardware claim is made.\n\n- FPGA family: {family}\n- Development tool baseline: {tool}; pin the exact version in the consuming project\n- Reference fabric clock: 100 MHz\n- ADS1299 serial mode: SPI Mode 1, conservative 4 MHz target\n- ADS1299 count: one in the reference; replicate CS/DRDY and budget timing for more\n- Data path: byte transaction controller -> 15/21/27-byte parser -> application FIFO/DMA\n- FIFO/DMA: integration boundary documented; no vendor FIFO/IP is bundled\n\nThe RTL transports opaque bytes and parses 4/6/8-channel frames. Register and\nconfiguration policy stays in the shared ADS1299 Core or a host sequencer. The\nconstraint file is a template because no exact board revision is claimed.\n""")
        write(base / "rtl" / "ads1299_spi_master.v", SPI_MASTER)
        write(base / "rtl" / "ads1299_controller.v", CONTROLLER)
        write(base / "rtl" / "frame_parser.v", FRAME_PARSER)
        write(base / "tb" / "simulation_tb.v", TB)
        suffix = {"XDC": "xdc", "QSF": "qsf", "LPF": "lpf", "PDC": "pdc",
                  "CST": "cst", "ADC": "adc", "SDC": "sdc"}[constraint_kind]
        write(base / "constraints" / f"pins.{suffix}", f"""# {constraint_kind} constraint template for {family}.\n# Assign clk, rst_n, sclk, mosi, miso, cs_n, drdy_n, reset_n and start\n# from the exact board's official master constraints. No physical pins are\n# asserted here because this package does not claim a specific board revision.\n""")
        write(base / "integration.md", f"""# Integration\n\n1. Create a clean {tool} project for the exact board and part.\n2. Add the three files under `rtl/` and select the intended top-level wrapper.\n3. Copy the official board constraints; map clock and ADS1299 pins explicitly.\n4. Constrain generated SCLK and both input/output timing paths.\n5. Feed opaque command/register bytes from the shared Core or host sequencer.\n6. Put parsed frames into an async FIFO before USB, PCIe, Ethernet or DMA logic.\n7. Run `tb/simulation_tb.v`, synthesis and timing analysis before hardware use.\n8. Validate ID read and the internal test signal on real ADS1299 hardware.\n\nDo not commit tool caches, generated IP output, bitstreams or timing databases.\n""")
        write(base / "SOURCES.md", f"""# Sources and provenance\n\n- ADS1299 behavior and timing: Texas Instruments ADS1299-x datasheet SBAS499C.\n- FPGA tool and constraint syntax: official {vendor} documentation for {tool}.\n- RTL implementation: original code written for ADS1299-Core-SDK; it is not a copied vendor example or third-party GitHub implementation.\n\nNo vendor SDK, generated IP, startup project, cache or middleware is included.\nThe repository owner must select and publish a project-wide open-source license; this package does not silently impose one.\n""")
        write(base / "project.json", json.dumps({
            "schema": 1, "rank": rank, "domain": "fpga", "path": relative,
            "vendor": vendor, "family": family, "tool": tool,
            "constraint": f"constraints/pins.{suffix}", "status": "Reference",
        }, ensure_ascii=False, indent=2))
        write(base / "validation.md", f"""# Validation\n\nCurrent status: **Reference**\n\n- RTL source present: yes\n- Self-checking testbench present: yes\n- Simulator result: not run on this machine\n- Synthesis: not run\n- Timing closure: not run\n- Bench-tested: no\n- 24h-tested: no\n""")
    index = [
        "# ADS1299 FPGA interface layer", "",
        "Ranks 71-85 are FPGA/CPLD interfaces and remain separate from MCU ports.", "",
        "| Rank | Vendor | Family | Package | Status |",
        "|---:|---|---|---|---|",
    ]
    for rank, relative, vendor, family, _tool, _constraint in TARGETS:
        index.append(f"| {rank} | {vendor} | {family} | `{relative}` | Reference |")
    index.extend([
        "", "Every package contains the same vendor-neutral Mode-1 SPI byte engine,",
        "transaction controller, 4/6/8-channel frame parser and self-checking",
        "testbench. Only constraint syntax and integration documentation differ.", "",
        "Register definitions and configuration policy remain in `core_driver/ads1299`.",
        "No package claims simulation, synthesis, timing closure or hardware testing.",
    ])
    write(FPGA / "README.md", "\n".join(index))
    print(f"generated {len(TARGETS)} FPGA reference packages")


if __name__ == "__main__":
    main()
