const express = require('express');
const http = require('http');
const mqtt = require('mqtt');
const { Server } = require('socket.io');
const path = require('path');

const app = express();
const server = http.createServer(app);
const io = new Server(server);

// Servir archivos estáticos (el frontend)
app.use(express.static(path.join(__dirname, 'public')));

// 1. Conexión al Broker MQTT
// Docker nos permite usar el nombre del servicio como URL gracias a su DNS interno
const mqttClient = mqtt.connect('mqtt://mosquitto');
const topic = 'riego/sensores';

mqttClient.on('connect', () => {
    console.log('Conectado al Broker MQTT público');
    mqttClient.subscribe(topic, (err) => {
        if (!err) console.log(`Suscrito al tópico: ${topic}`);
    });
});

// 2. Recepción de datos del ESP32 y retransmisión por WebSockets
mqttClient.on('message', (topic, message) => {
    try {
        const datos = JSON.parse(message.toString());
        console.log('Datos recibidos del hardware:', datos);

        // Aquí es donde meterías el query SQL para guardar en tu Base de Datos Relacional:
        // db.query('INSERT INTO lecturas ...')

        // Enviamos el dato en tiempo real al Dashboard web
        io.emit('datos-sensor', datos);
    } catch (e) {
        console.log('Error al parsear el JSON:', e.message);
    }
});

// Iniciar servidor web en el puerto 3000
server.listen(3000, () => {
    console.log('Servidor corriendo en http://localhost:3000');
});