local wifi_config = {}
wifi_config.ssid = "twguest"
wifi_config.pwd = "password of the day"

local mqtt_client = mqtt.Client('farmer.cloudmqtt.com',
                                120, 'hgjpspdi', '_Tb2bO6s_sc7')

mqtt_client:on("connect", function(client) print ("connected") end)
mqtt_client:on("offline", function(client) print ("offline") end)

function connect_to_wifi(ssid, password)
  wifi.setmode(wifi.STATION)
  wifi.sta.config(wifi_config)
  wifi.sta.connect()
end

function wait_on_ip(callback)
  tmr.create():alarm(1000, tmr.ALARM_AUTO,
  function(t)
    if wifi.sta.getip() ~= nil then
      t:unregister()
      callback()
      else print("not connected")
    end
  end)
end

function poll_GPIO_3()
   tmr.create():alarm(1000, tmr.ALARM_AUTO,
                      function(t)
                         if gpio.read(3) == 1 then
                            print("open")
                         else
                            print("closed")
                         end
                     end)
end


function on_mqtt_subscribe(client)
   print("connected to MQTT")
   client:subscribe("/lua", 0,
                    function (c) print ("subscribe success") end)
   client:publish("/lua", "hello", 0, 0,
                  function(client) print("sent") end)
end

function continue_with_wifi()
   mqtt_client:connect("farmer.cloudmqtt.com", 12892, false, on_mqtt_subscribe)
   poll_GPIO_3 ()
end

gpio.mode (3, gpio.INPUT, gpio.PULLUP)



connect_to_wifi(ssid, password)

wait_on_ip(continue_with_wifi)
