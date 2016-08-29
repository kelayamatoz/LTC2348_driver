import serial
import MySQLdb
import sys
import atexit
import time
def save_exit():
    _conn.commit()
    _conn.close()
    _cur.close()

global _conn
global _cur
_conn = MySQLdb.connect(host='localhost', user='root', passwd='raspberry', db='mysql')
conn_re = 0
_cur = _conn.cursor()
atexit.register(save_exit)
if len(sys.argv) == 1:
    print 'usage: python streaming_database.py [test/daq]'
    print 'test is the testing database table, daq is the actual database table'
    sys.exit()

tab_string = "USE " + sys.argv[1]
_cur.execute(tab_string)
conn_re = conn_re | _cur.execute(tab_string)
if conn_re:
    print 'error: cannot execute ', tab_string
with serial.Serial(port='/dev/ttyUSB0',baudrate=230400,timeout=1) as ser:
    # cleanup the database everytime the program is starting from fresh
    cmd_string = "DROP TABLE data;"
    _cur.execute(cmd_string)
    cmd_string = "CREATE TABLE data (channel_no int, \
            channel_reading float, channel_time double);"
    _cur.execute(cmd_string)
    # disregard the first few readlines as the numbers can be corrupted
    for i in range(0,8):
        ser.readline()
    start = time.time()
    while 1:
        serial_line = ser.readline()
        cur_time = time.time() - start
        readings = serial_line.split(':')[0:8]
        if len(readings) != 8:
            continue
        for i in range(0,8):
            reading = readings[i]
            channel = i + 1
            cmd_string = "INSERT INTO data VALUES (" + str(channel) + ", " \
                    + reading + ", " + str(cur_time) + ");"
            try:
                _cur.execute(cmd_string)
                _conn.commit()
            except :
                _conn.rollback()
                pass

