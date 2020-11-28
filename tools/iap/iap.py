import sys
import os
import serial
import crc8
import time

config_dir = {"packet_size": 512, "com": "COM3", "baudrate": 115200}
image_file = "firmware.bin"


def iap_start(__serial):
    data = __serial.read(size=len("update request") * 2)
    # print(type(data))
    if data.decode("utf-8").find("update request") >= 0:
        __serial.write("update".encode("utf-8"))
        __serial.flush()
        return 1
    return 0


def send_packet(__serial):
    org_data = b''
    index = 0
    tmp_str = ""

    with open(image_file, "rb") as fd:
        tmp_str = ""
        send_cnt = 0

        org_data = fd.read(-1)
        hash = crc8.crc8()
        hash.update(org_data)
        #total_crc = hash.hexdigest()
    while True:
        if len(org_data) >= 512:
            hash = crc8.crc8()
            # hash.update(b'\x7E')
            # hash.update(bytes([index & 0xff]))
            # hash.update(bytes([index >> 8]))
            # hash.update(bytes([00]))
            # hash.update(bytes([2]))
            hash.update(org_data[0:512])
            tmp_str = "7E" + \
                '{:0>2x}'.format(index & 0xff)+"{:0>2x}".format(index >> 8) + "{:0>2x}".format(0x00)+ \
                "{:0>2x}".format(2) + org_data[0:512].hex()+hash.hexdigest()
            org_data = org_data[512:]
            index += 1
        else:
            __len = len(org_data[0:]) + 1
            hash = crc8.crc8()
            # hash.update(b'\x7E')
            # hash.update(bytes([index & 0xff]))
            # hash.update(bytes([index >> 8]))
            # hash.update(bytes([__len & 0xff]))
            # hash.update(bytes([__len >> 8]))
            hash.update(org_data)
            # hash.update(bytes.fromhex(total_crc))
            tmp_str = "7E" + "{:0>2x}".format(index & 0xff) + \
                "{:0>2x}".format(index >> 8) + "{:0>2x}".format(__len & 0xff) + \
                "{:0>2x}".format(__len >> 8) + \
                org_data.hex() + hash.hexdigest()
            break

        __serial.write(bytes.fromhex(tmp_str))
        __serial.flush()
        send_cnt += 1
        print(send_cnt)
        print(tmp_str)
        time.sleep(0.05)
    return -1


def proc_data():
    packet_sz = int(config_dir["packet_size"])
    func_list = [iap_start, send_packet]
    index = 0

    __serial = serial.Serial(port=config_dir["com"],
                             baudrate=int(config_dir["baudrate"]))
    print("open serial:%s,baudrate:%d" %
          (config_dir["com"], int(config_dir["baudrate"])))
    '''try:
        while (index >= 0):
            index = func_list[index](__serial)
    except:
        __serial.close()
        return
    '''
    while ((index >= 0) and (index < len(func_list))):
        index = func_list[index](__serial)
    __serial.close()
    return


if __name__ == "__main__":
    #try:
    proc_data()
    #except:
    #    sys.exit(0)
