//Exposes a class for interfacing with the MAX31855 thermocouple amplifier using SPI.

#include "max31855.h" 
#include <esp_log.h>


static const char *TAG = "MAX31855 Driver";


MAX31855::MAX31855(int miso_pin, int mosi_pin, int clk_pin, int cs_pin)
{
    if(!this->host_device)
    {  
        if(spicommon_periph_claim(HSPI_HOST))
        {
            this->host_device = HSPI_HOST;
        }
        else if(spicommon_periph_claim(VSPI_HOST))
        {
            this->host_device = VSPI_HOST;
        }
        else
        {
            ESP_LOGD(TAG, "SPI Driver could not claim any host device!");
            return;
        }
        spi_bus_config_t bus_config;
        bus_config.mosi_io_num = mosi_pin;
        bus_config.miso_io_num = miso_pin;
        bus_config.sclk_io_num = clk_pin;
        bus_config.quadwp_io_num = -1;
        bus_config.quadhd_io_num = -1;
        bus_config.max_transfer_sz = 10;
        bus_config.flags = SPICOMMON_BUSFLAG_MASTER;
        spi_bus_initialize(this->host_device);
        ESP_ERROR_CHECK()
    }
}

//INCOMPLETE