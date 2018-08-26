#include "driver/include/driver/spi_master.h"

 class MAX31855
 {
    MAX31855::MAX31855(spi_host_device_t spi_host_device, int cs_gpio_pin);
    
    private:
        spi_device_handle_t device_handle;
        static spi_host_device_t host_device = 0;

 }