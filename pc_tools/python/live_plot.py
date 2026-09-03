"""Simple live 8-channel viewer for the common ADS1299-Core host protocol.

This is an engineering viewer, not a clinical EEG display.
"""

from __future__ import annotations
import argparse
from collections import deque
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from serial_reader import packets_from_serial


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("port")
    ap.add_argument("--baud", type=int, default=921600)
    ap.add_argument("--fs", type=float, default=250.0)
    ap.add_argument("--seconds", type=float, default=5.0)
    args = ap.parse_args()

    capacity = max(50, int(round(args.fs * args.seconds)))
    buffers = [deque([0.0]*capacity, maxlen=capacity) for _ in range(8)]
    stream = packets_from_serial(args.port, args.baud)

    fig, ax = plt.subplots()
    x = np.arange(capacity) / args.fs - args.seconds
    lines = [ax.plot(x, np.zeros(capacity), label=f"CH{i+1}")[0] for i in range(8)]
    ax.set_xlabel("Time (s)")
    ax.set_ylabel("ADC code + display offset")
    ax.set_title("ADS1299-Core live viewer")

    def update(_):
        for _ in range(max(1, int(args.fs/20))):
            frame = next(stream)
            for ch, value in enumerate(frame.channels):
                buffers[ch].append(float(value))
        raw = np.asarray([list(b) for b in buffers])
        scale = max(np.nanstd(raw), 1.0)
        for ch, line in enumerate(lines):
            line.set_ydata(raw[ch] + ch * 8.0 * scale)
        ax.relim()
        ax.autoscale_view(scalex=False, scaley=True)
        return lines

    FuncAnimation(fig, update, interval=50, blit=False, cache_frame_data=False)
    plt.show()


if __name__ == "__main__":
    main()
