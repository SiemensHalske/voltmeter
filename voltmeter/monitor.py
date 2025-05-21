import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque


class SerialVoltagePlotter:
    def __init__(self, port, baudrate, expected_token, buffer_size=100):
        self.port = port
        self.baudrate = baudrate
        self.expected_token = expected_token
        self.buffer_size = buffer_size

        self.serial = serial.Serial(self.port, self.baudrate, timeout=1)
        self.data_buffer = deque(
            [0.0] * self.buffer_size, maxlen=self.buffer_size)

        self.fig, self.ax = plt.subplots()
        self.line, = self.ax.plot([], [], lw=2)

        self._configure_plot()

        self.ani = animation.FuncAnimation(
            self.fig, self._update, interval=100)
        plt.tight_layout()

    def _configure_plot(self):
        self.ax.set_ylim(0, 5.1)
        self.ax.set_xlim(0, self.buffer_size - 1)
        self.ax.set_title("Live Voltage & Frequency from Arduino")
        self.ax.set_xlabel("Sample Index")
        self.ax.set_ylabel("Voltage (V)")
        # reserve a spot for frequency text
        self.freq_text = self.ax.text(
            0.02, 0.95, "", transform=self.ax.transAxes, va="top")

    def _update(self, frame):
        # update the line data
        self.line.set_ydata(self.data_buffer)

        try:
            raw_line = self.serial.readline().decode('utf-8').strip()
            if raw_line:
                parts = raw_line.split(',')
                # now expecting 3 parts: token, mV, frequency
                if len(parts) == 3 and parts[0] == self.expected_token:
                    mV = int(parts[1])
                    volts = mV / 1000.0
                    freq = float(parts[2])

                    # push new voltage into the buffer
                    self.data_buffer.append(volts)

                    # update title or text with both values
                    self.ax.set_title(
                        f"Voltage: {volts:.3f} V    Frequency: {freq:.2f} Hz")
                    # or, if you prefer it below:
                    # self.freq_text.set_text(f"Freq: {freq:.2f} Hz")
        except (ValueError, UnicodeDecodeError):
            # just skip malformed lines
            pass

        # ensure x-data matches buffer length
        self.line.set_xdata(range(len(self.data_buffer)))
        return self.line

    def run(self):
        plt.show()

    def close(self):
        self.serial.close()


def main():
    EXPECTED_TOKEN = "YTNlYzQ5MDJlMjY0YTcwNjY1YjgyODVkNDkyNDdiOTZhODc1ZmVhOTExNDU4N2VhZjA3YjFkMDEwZjhmZGVkYTEyMGZkZjQ1MGU5ZTczYzc4MDJiNzJjNTZhNWFjY2M3NjViZDRlNDY5ZDhmMjljZGJiMjA2Y2NjZjUxMTlmODg="
    plotter = SerialVoltagePlotter(
        port="/dev/ttyUSB0",
        baudrate=9600,
        expected_token=EXPECTED_TOKEN,
        buffer_size=100
    )

    try:
        plotter.run()
    finally:
        plotter.close()


if __name__ == "__main__":
    main()
