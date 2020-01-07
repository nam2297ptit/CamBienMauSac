const express = require("express");
const config = require("./config");
const ejsLint = require("ejs-lint");
const app = express();
app.use(express.static("views"));
app.use(express.static("static"));
app.set("view engine", "ejs");
// Khởi tạo server
var server = require("http")
    .createServer(app)
    .listen(config.app.port, function() {
        console.log("[Server] Running on http://localhost:" + config.app.port);
    });

//Router
app.get("/", function(req, res) {
    res.render("./pages/index", { title: "home" });
});

//Socket
const io = require("socket.io").listen(server);
io.on("connection", function(socket) {
    socket.on("disconnect", function() {
        console.log("[Socket] A user disconnected");
    });
});

//MQTT
const mqtt = require("mqtt");
var client = mqtt.connect(config.server_mqtt, config.options_mqtt);
client.on("connect", function() {
    console.log("[MQTT][Connect] Connected to MQTT server!");
    client.subscribe("#", function() {
        client.on("message", function(topic, message, packet) {
            console.log("Received '" + message + "' on '" + topic + "'");
            //Gửi dữ liệu màu sắc lên server qua socket
            if (topic === "color") {
                io.emit("color", message.toString());
            } else if (topic === "product") {
                io.emit("product", message.toString());
            }
            else if (topic === "sensor") {
                io.emit("sensor", message.toString());
            }
        });
    });
});

// //MYSQL
// var mysql = require("mysql");
// var time = new Date();
// var con = mysql.createConnection(config.options_mysql);
// con.connect(function(err) {
//     if (err) throw err;
// console.log("[MYSQL][Connect] Connected to MYSQL database!")
//     var sql = "INSERT INTO product (color, time) VALUES (?, ?)"
 
// 	client.subscribe("#", function() {
//         client.on("message", function(topic, message, packet) {
//             if (topic === "product") {
// 				var val= [message,new Date()];
// 				con.query(sql, val, function (err, result) {
// 					  if (err) throw err;
// 					  console.log("1 record inserted");
					
// 				});
//             } 
//         });
//     });
// });
