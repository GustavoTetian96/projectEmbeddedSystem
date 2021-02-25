import time
import serial
import mysql.connector
from mysql.connector import Error
import os
import io
temperatura=0
def insertVariblesIntoTable(nombre,apellido,temp):
    try:
        connection = mysql.connector.connect(host='bzipqoks52bmmxqwmsya-mysql.services.clever-cloud.com',
                                             database='bzipqoks52bmmxqwmsya',
                                             user='uyr3cmeq2xz1wuq8',
                                             password='CdtkLnRIBILdYGZe9DDJ')
        cursor = connection.cursor()
        mySql_insert_query = """INSERT INTO personas (Nombre,Apellido,Temperatura) 
                                VALUES (%s, %s, %s) """

        recordTuple = (nombre,apellido,temp)
        #cursor.execute("""DROP TABLE IF EXISTS personas""")
        #cursor.execute("""CREATE TABLE personas(ID int not null auto_increment primary key,Nombre varchar(25),Apellido varchar(25),Temperatura int,fechaHora timestamp default now())""")
        cursor.execute(mySql_insert_query, recordTuple)
        connection.commit()
        print("insercion de datos exitosa")

    except mysql.connector.Error as error:
        print("Failed to insert into MySQL table {}".format(error))

    finally:
        if (connection.is_connected()):
            cursor.close()
            connection.close()
            print("MySQL connection is closed")


arduino = serial.Serial(
        port='/dev/ttyS1',
        baudrate = 9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)
print("Conectado por serial..")
arduino.flush()
try:
	while(1):
		if(arduino.in_waiting>0):
			temperatura=arduino.readline(3)
			print(temperatura)
			arduino.write(str(1).encode('utf-8'))
			time.sleep(1)
			if float(temperatura)>=370:
				print("ALERTA!! la temperatura es:",int(temperatura))
				nombre=input("Ingrese Nombre:  ")
				apellido=input("Ingrese Apellido: ")
				print(nombre,apellido)
				insertVariblesIntoTable(nombre,apellido,temperatura)
			
				temperatura=arduino.readline(3)
				#print("Tablas de 'ciudadela':")
				#for fila in filas:
				#     print(fila)
			else:
				print("Temperatura normal: ",temperatura)

except(KeyboardInterrup,SystemExit):
	print()
	print("hasta luego")
	arduino.close()
