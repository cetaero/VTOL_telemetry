//This is test 1 for our esp-now receiver. What it does is that it sends a custom string like "hello world" to the receiver
//esp32 and checks if it is recived via uart


#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

