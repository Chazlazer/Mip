import socket
import struct
import time
import select
import subprocess

STRUCT_FORMAT = "8B6B"
STRUCT_SIZE = struct.calcsize(STRUCT_FORMAT)

last_command = {
    "BUTTON_A": 0,
    "BUTTON_B": 0,
    "BUTTON_X": 0,
    "BUTTON_Y": 0,
    "BUTTON_L": 0,
    "BUTTON_R": 0,
    "BUTTON_WL": 0,
    "BUTTON_WR": 0,
    "JOYSTICK_L_X": 127,
    "JOYSTICK_L_Y": 127,
    "JOYSTICK_R_X": 127,
    "JOYSTICK_R_Y": 127,
    "TRIGGER_L": 0,
    "TRIGGER_R": 0
}

server_sock = None
client_sock = None

def get_bdaddr():
    try:
        result = subprocess.run(["hciconfig"], capture_output=True, text=True)
        for line in result.stdout.split("\n"):
            if "BD Address" in line:
                return line.split("BD Address:")[1].strip().split(" ")[0]
    except Exception as e:
        print("Error getting Bluetooth address:", e)
    return None

def parse_commands(data: bytes):
    values = struct.unpack(STRUCT_FORMAT, data)
    return {
        "BUTTON_A": values[0],
        "BUTTON_B": values[1],
        "BUTTON_X": values[2],
        "BUTTON_Y": values[3],
        "BUTTON_L": values[4],
        "BUTTON_R": values[5],
        "BUTTON_WL": values[6],
        "BUTTON_WR": values[7],
        "JOYSTICK_L_X": values[8],
        "JOYSTICK_L_Y": values[9],
        "JOYSTICK_R_X": values[10],
        "JOYSTICK_R_Y": values[11],
        "TRIGGER_L": values[12],
        "TRIGGER_R": values[13]
    }

def print_commands(cmd):
    print(f"A:{cmd['BUTTON_A']} B:{cmd['BUTTON_B']} X:{cmd['BUTTON_X']} Y:{cmd['BUTTON_Y']} "
          f"LX:{cmd['JOYSTICK_L_X']} LY:{cmd['JOYSTICK_L_Y']} "
          f"RX:{cmd['JOYSTICK_R_X']} RY:{cmd['JOYSTICK_R_Y']} "
          f"TL:{cmd['TRIGGER_L']} TR:{cmd['TRIGGER_R']}")

def init_server():
    global server_sock, client_sock
    bdaddr = get_bdaddr()
    if not bdaddr:
        raise RuntimeError("Failed to get local Bluetooth address.")

    server_sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
    server_sock.bind((bdaddr, 4))
    server_sock.listen(1)
    server_sock.setblocking(False)
    print(f"Waiting for Bluetooth connection on {bdaddr}, channel 4...")

    while client_sock is None:
        rlist, _, _ = select.select([server_sock], [], [], 0.1)
        if rlist:
            client_sock, client_info = server_sock.accept()
            client_sock.setblocking(False)
            print("Accepted connection from", client_info)

def poll_data():
    """Polls once for controller data."""
    global last_command
    if client_sock is None:
        return last_command
    try:
        data = client_sock.recv(STRUCT_SIZE)
        if len(data) == STRUCT_SIZE:
            last_command = parse_commands(data)
    except BlockingIOError:
        pass
    return last_command

def get_last_command_dict():
    return last_command

# Original behavior for direct run
def main():
    init_server()
    while True:
        poll_data()
        print_commands(last_command)
        time.sleep(0.005)

if __name__ == "__main__":
    main()
