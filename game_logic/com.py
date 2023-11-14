import serial


class PrintLines(LineReader):
    def connection_made(self, transport):
        super(PrintLines, self).connection_made(transport)
        sys.stdout.write('port opened\n')

    def handle_line(self, data):
        sys.stdout.write('line received: {}\n'.format(repr(data)))

    def connection_lost(self, exc):
        if exc:
            traceback.print_exc(exc)
        sys.stdout.write('port closed\n')


def start_listening():
    ser = serial.serial_for_url('/dev/tty', baudrate=115200, timeout=1)
    with ReaderThread(ser, PrintLines) as protocol:
        ## protocol.write_line('hello')
        time.sleep(2)
