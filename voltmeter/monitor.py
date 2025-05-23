import serial


class SerialVoltagePrinter:
    lastLine: str = ''

    def __init__(self, port, baudrate):
        self.port = port
        self.baudrate = baudrate
        self.serial = serial.Serial(self.port, self.baudrate, timeout=1)

    def run(self):
        try:
            while True:
                try:
                    raw_line = self.serial.readline().decode('utf-8').strip()
                    if raw_line:
                        # line example
                        # Voltage: 1.642 V | Frequency: 8.35 Hz
                        # only print if the values are different.
                        if raw_line != self.lastLine:
                            print(raw_line)
                            self.lastLine = raw_line
                except (ValueError, UnicodeDecodeError):
                    pass
        except KeyboardInterrupt:
            pass

    def close(self):
        self.serial.close()


def main():
    printer = SerialVoltagePrinter(
        port="/dev/ttyUSB0",
        baudrate=9600
    )

    try:
        printer.run()
    finally:
        printer.close()


if __name__ == "__main__":
    main()
