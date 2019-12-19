package main

import (
	"fmt"
	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/go-playground/log"
	"time"
)

const TOPIC = "/feeds/door"
const THRESHOLD = 10

var done = make(chan bool)
var open = false

func main() {

	log.Info("starting ...")

	c := createClient()
	connect(c)

	forever := make(chan bool)
	go listen(c)
	<-forever

}

func createClient() mqtt.Client {
	log.Info("creating client ...")

	opts := mqtt.NewClientOptions().AddBroker("farmer.cloudmqtt.com:10812")
	opts.SetUsername("...")
	opts.SetPassword("...")
	return mqtt.NewClient(opts)
}

func connect(c mqtt.Client) {
	log.Info("trying to connect ...")
	if token := c.Connect(); token.Wait() && token.Error() != nil {
		log.Error("Error when trying to connect", token.Error())
	}
	/*	token := c.Connect()
		for !token.WaitTimeout(3 * time.Second) {
		}
		if err := token.Error(); err != nil {
			log.Fatal(err)
		}*/
}

func listen(c mqtt.Client) {

	connect(c)
	log.Info("subscribing to channel " + TOPIC)
	c.Subscribe(TOPIC, 0, func(client mqtt.Client, msg mqtt.Message) {
		log.Info("Received value " + string(msg.Payload()) + " from channel " + TOPIC)

		if string(msg.Payload()) == "1" && open == false {
			done = make(chan bool)
			open = true
			counter := 1
			time.Sleep(THRESHOLD * time.Second)
			handleOpenDoor(counter)
		} else if string(msg.Payload()) == "1" && open == true {
			log.Info("Door is still open")
		} else if string(msg.Payload()) == "0" {
			log.Info("Door is closed again")
			open = false
			close(done)
		} else {
			log.Info("Unknown message. ")
		}
	})
}

func handleOpenDoor(counter int) {
	log.Info("Door is open... Sven will handle this!")

	go func() {
		ticker := time.NewTicker(10 * time.Second)
		for {
			select {
			case <-done:
				log.Info("done")
				ticker.Stop()
				return
			case <-ticker.C:
				counter++
				fmt.Print("Trigger Warning #")
				fmt.Println(counter)
			}
		}
	}()
}