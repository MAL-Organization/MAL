import time

import serial


def main():
    with serial.Serial('COM5', 115200, timeout=1) as ser:
        test_time = 10
        test_byte = 0
        chunk_size = 16
        elapsed_time = 0
        start_time = time.monotonic()
        error_count = 0
        while elapsed_time < test_time:
            elapsed_time = time.monotonic() - start_time
            # Write bytes
            test_bytes = list()
            for i in range(chunk_size):
                current_test_byte = (test_byte + i) & 0xFF
                test_bytes.append(current_test_byte)
            ser.write(bytes(test_bytes))
            # Wait for loopback
            for i in range(chunk_size):
                rx = ser.read(size=1)
                if len(rx) != 1:
                    error_count += 1
                    print("Failed to loopback {}".format(test_byte))
                    continue
                else:
                    rx_int = int(rx[0])
                    print("Received {}".format(rx_int))
                    if (test_byte + i) != rx_int:
                        error_count += 1
                        print("Sent {} but received {}".format(test_byte, rx_int))
            # Wrap test byte
            test_byte = (test_byte + chunk_size) & 0xFF
        print("Finished with {} error(s)".format(error_count))


if __name__ == '__main__':
    main()
