
import time
import serial
import mysql.connector
from mysql.connector import Error
import os
import io
import requests
import json
temperatura=0

def insertVariblesIntoTable(nombre,apellido,temp):
    try:
        connection = mysql.connector.connect(host='localhost',
                                             database='ciudadelaDB',
                                             user='gustavoCastillo',
                                             password='123')
        cursor = connection.cursor()
        mySql_insert_query = """INSERT INTO personas (Nombre,Apellido,Temperatura) 
                                VALUES (%s, %s, %s) """

        recordTuple = (nombre,apellido,temp)
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
			temperatura=arduino.readline(2).strip()
			print(temperatura)
			enviar = requests.get("https://api.thingspeak.com/update?api_key=WCLYG0G6ALZJXKK0&field1="+str(temperatura.decode()))
			if float(temperatura)>=37:
				print("ALERTA!! la temperatura es:",int(temperatura))
				nombre=input("Ingrese Nombre:  ")
				apellido=input("Ingrese Apellido: ")
				print(nombre,apellido)
				insertVariblesIntoTable(nombre,apellido,temperatura)
			
				temperatura=arduino.readline(2).strip()
				#print("Tablas de 'ciudadela':")
				#for fila in filas:
				#     print(fila)
			else:
				print("Temperatura normal: ",temperatura)
				temperatura=arduino.readline(2).strip()
			
			if enviar.status_code == requests.codes.ok:
				if enviar.text != '0':
					print("Datos enviados correctamente")
				else:
					print("Tiempo de espera insuficiente (>15seg)")
			else:
				print("Error en el request: ",enviar.status_code)
			recibir=requests.get("https://api.thingspeak.com/channels/7NUASKBQJLYQCE67/feeds.json")
			jsonString=json.dumps(recibir.json(),indent=2)
			time.sleep(15)
except(KeyboardInterrup,SystemExit):
	print()
	print("hasta luego")
	arduino.close()
